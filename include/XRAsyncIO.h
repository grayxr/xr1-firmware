#ifndef XRAsyncIO_h
#define XRAsyncIO_h

#include <Arduino.h>
#include <XRSD.h>
#include <string>
#include <queue>
#include <functional>
#include <memory>
#include <XRSequencer.h>
#include <XRSound.h>

//TODO: define a higher read speed when doing manual bnk/ptn/layer changes
// use 512 * 8 for when in automated (song / chain / auto fill) modes
#define ASYNC_IO_R_BUFFER_SIZE 512 * 8

// always use this write size no matter the mode?
#define ASYNC_IO_W_BUFFER_SIZE 512 * 8

namespace XRAsyncIO
{
    enum FILE_IO_STATE
    {
        IDLE = 0,
        START,
        BUSY,
        COMPLETE,
        ERROR
    };

    enum FILE_IO_TYPE
    {
        READ = 0,
        WRITE
    };

    enum FILE_TYPE {
        PATTERN_SETTINGS = 0,
        TRACK_LAYER,
        RATCHET_LAYER,
        KIT,
    };

    typedef struct
    {
        FILE_TYPE fileType;
        FILE_IO_TYPE ioType;
        std::string filename;
        uint32_t size;
        std::function<void(const FILE_TYPE)> failedCb;
        std::function<void(const FILE_TYPE)> successCb;
    } IO_CONTEXT;

    typedef struct {
        uint32_t total;
        int8_t *index;
    } READ_IO;

    typedef struct {
        uint32_t offset;
        uint32_t remaining;
        uint8_t buffer[ASYNC_IO_W_BUFFER_SIZE];
    } WRITE_IO;

    void update();
    void addItem(IO_CONTEXT ctx);
    void processNextItem();
}

#endif /* XRAsyncIO_h */