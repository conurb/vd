#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fts.h>
#include <libavformat/avformat.h>


int64_t video_duration_in_seconds(const char *);
void duration_stringify(int64_t, char *, size_t);
int compare (const FTSENT**, const FTSENT**);


int main(int argc, char *const *argv)
{
    if (argc == 1) {
        fprintf(stderr, "usage: %s <file or directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FTS* file_system = NULL;
    FTSENT* entry    = NULL;
    int64_t duration = 0;

    file_system = fts_open(argv + 1, FTS_PHYSICAL | FTS_COMFOLLOW | FTS_NOCHDIR, &compare);

    if (NULL != file_system) {
        while(NULL != (entry = fts_read(file_system))) {
            if (FTS_F == entry->fts_info) {
                duration += video_duration_in_seconds(entry->fts_path);
            }
        }
        fts_close(file_system);
    }

    /**
     * buffer for textual representation of duration (hh:mm:ss)
     * buffer size of 23 will properly handle an INT64_MAX duration
     */
    char duration_hms[23] = {0};

    duration_stringify(duration, duration_hms, sizeof(duration_hms));
    puts(duration_hms);
}

int compare(const FTSENT** one, const FTSENT** two)
{
    return (strcmp((*one)->fts_name, (*two)->fts_name));
}

void duration_stringify(int64_t duration_in_seconds, char *buffer, size_t buffer_size)
{
    if (!buffer || buffer_size < 1 || duration_in_seconds < 1)
        return;

    int64_t h = duration_in_seconds / 3600;
    int64_t m = duration_in_seconds / 60 % 60;
    int64_t s = duration_in_seconds % 60;

    snprintf(buffer, buffer_size, "%02" PRId64 ":%02" PRId64 ":%02" PRId64 "", h, m, s);
}

int64_t video_duration_in_seconds(const char *filename)
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
