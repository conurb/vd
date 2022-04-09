# vd
sum videos' durations in a file hierarchy

## build dependencies
- pkg-config
- ffmpeg

### on mac using homebrew
```bash
$ brew install pkg-config
$ brew install ffmpeg
```

## build
```bash
$ make
```

## usage
eg:
```bash
$ ./vd ~/Movies/Action ~/Movies/Info ~/Movies/example.mkv
54:02:23
```
