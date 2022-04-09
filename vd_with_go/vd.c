#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fts.h>
#include <libavformat/avformat.h>
#include "vd.h"

static int compare (const FTSENT**, const FTSENT**);
static int64_t duration_in_seconds(const char *);

static int compare(const FTSENT** one, const FTSENT** two)
{
    return (strcmp((*one)->fts_name, (*two)->fts_name));
}

static int64_t duration_in_seconds(const char *filename)
{
    int64_t duration = 0;

    av_log_set_level(AV_LOG_QUIET);
    AVFormatContext *p_format_context = avformat_alloc_context();

    if (0 == avformat_open_input(&p_format_context, filename, NULL, NULL)) {
        for (size_t i = 0; i < p_format_context->nb_streams; i++) {
            AVCodecParameters *codecpar = p_format_context->streams[i]->codecpar;

            /**
             * get rid of images:
             * codecpar->width is only used for videos
             * codecpar->width returns 0 for an image
             */
             if (codecpar->width && AVMEDIA_TYPE_VIDEO == codecpar->codec_type) {
                duration = p_format_context->duration / AV_TIME_BASE;
                break;
            }
        }
        avformat_close_input(&p_format_context);
    }

    avformat_free_context(p_format_context);

    return duration;
}

int64_t duration_sum_in_file_hierarchy(char **path)
{
    if (NULL == *path)
        return 0;

    FTS* file_system = NULL;
    FTSENT* entry    = NULL;
    int64_t duration = 0;

    file_system = fts_open(path, FTS_PHYSICAL | FTS_COMFOLLOW | FTS_NOCHDIR, &compare);

    if (NULL != file_system) {
        while(NULL != (entry = fts_read(file_system))) {
            if (FTS_F == entry->fts_info) {
                duration += duration_in_seconds(entry->fts_path);
            }
        }
        fts_close(file_system);
    }

    return duration;
}
