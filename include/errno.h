

#ifndef ERRNO_H
#define ERRNO_H

enum errno {
    NO_ERR = 0,
    ERR_ARG,
    ERR_IO,
    ERR_BUSY,
    ERR_AGAIN,
    ERR_THREAD_DETACHED,
    ERR_NOT_BLOCKED,
    ERR_TIMED_OUT,
    ERR_OBJECT_DESTORYED,
};

/*
const char* err_str[] = {
    {"no err"},
    {"err arg"},
    {"err io"},
    {"err busy"},
    {"err again"},
    {"err thread detached"},
    {"err not blocked"},
    {"err timed out"},
    {"err object destoryed"},
};
*/

#endif // ERRNO_H
