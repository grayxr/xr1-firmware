#include <XRAsyncIO.h>

namespace XRAsyncIO
{
    FILE_IO_STATE _state = IDLE;
    File _file;
    
    std::queue<IO_CONTEXT> _ioQueue;
    
    READ_IO _readIO;
    WRITE_IO _writeIO;

    void openNextItem();

    bool openForRead(IO_CONTEXT& ctx);
    bool openForWrite(IO_CONTEXT& ctx);
    bool doneReading(IO_CONTEXT& ctx, void *buf);
    bool doneWriting(IO_CONTEXT& ctx, const byte *buf);

    void *getReadObject(FILE_TYPE fileType);
    byte *getWriteObject(FILE_TYPE fileType);

    void update()
    {
        switch (_state)
        {
            case IDLE:
                if (!_ioQueue.empty()) { // there are items to process
                    _state = START;
                } else {
                    if (XRSD::writeState != XRSD::WRITE_STATE::IDLE) {
                        //Serial.println("CHANGING SD WRITE STATE TO IDLE NOW!");
                        XRSD::writeState = XRSD::WRITE_STATE::IDLE;
                    }
                }
                break;
            case START:
                openNextItem(); // open the next item's file in the queue
                break;
            case BUSY:
                processNextItem();
                break;
            case COMPLETE:
                _state = IDLE;
                XRSD::writeState = XRSD::WRITE_STATE::COMPLETE;
                        //Serial.println("CHANGING SD WRITE STATE TO COMPLETE NOW!");
                break;
            case ERROR:
                Serial.println("ASYNC IO ERROR!");
                _file.close();
                _state = IDLE;
                break;
        }
    }

    void addItem(const IO_CONTEXT ctx)
    {
        _ioQueue.push(ctx);
    }

    void openNextItem()
    {
        if (_ioQueue.empty()) return;

        if (XRSD::sdBusy()) {
            Serial.println("SD CARD BUSY! CANNOT OPEN!");
            
            return;
        }

        IO_CONTEXT& ctx = _ioQueue.front();

        switch (ctx.ioType)
        {
            case READ:
                _readIO.index = (int8_t*)getReadObject(ctx.fileType);
                _readIO.total = 0;

                Serial.printf("Opening file: %s, at index: %d\n", ctx.filename.c_str(), _readIO.index);

                if (!openForRead(ctx)) {
                    Serial.println("failed to open for read!");

                    if (ctx.failedCb != nullptr) {
                        Serial.println("calling present callback for failed read!");

                        ctx.failedCb(ctx.fileType);
                    }

                    _ioQueue.pop();
                    _state = START; // goto next file, assume failed read shouldn't block the queue
                } else {
                    _state = BUSY; // begin processing read
                }

                break;
            case WRITE:
                _writeIO.offset = 0;
                _writeIO.remaining = ctx.size;

                if (!openForWrite(ctx)) {
                    Serial.println("failed to open for write, retrying!");

                    _state = START; // retry if failed to open for write
                } else {
                    _state = BUSY; // begin processing write
                    XRSD::writeState = XRSD::WRITE_STATE::START;
                       // Serial.println("CHANGING SD WRITE STATE TO START NOW!");
                }

                break;
        }
    }

    void processNextItem()
    {
        if (_ioQueue.empty()) {
            Serial.println("ASYNC IO QUEUE EMPTY!");

            _state = COMPLETE; // all items processed, mark complete

            return;
        }

        if (XRSD::sdBusy()) {
            Serial.println("SD CARD BUSY!");
            
            return;
        }

        IO_CONTEXT& ctx = _ioQueue.front();

        switch (ctx.ioType)
        {
            case READ:
                if (!doneReading(ctx, getReadObject(ctx.fileType))) {
                    _state = BUSY; // continue reading
                } else {
                    _state = IDLE; // so we open the next file!

                    if (ctx.successCb != nullptr) {
                        Serial.println("calling present callback for successful read completed!");

                        ctx.successCb(ctx.fileType);
                    }

                    _ioQueue.pop();
                }

                break;
            case WRITE:
                if (!doneWriting(ctx, getWriteObject(ctx.fileType))) {
                    _state = BUSY; // continue writing
                } else {
                    _state = IDLE; // so we open the next file!
                    _ioQueue.pop();
                }

                break;
        }
    }

    bool openForRead(IO_CONTEXT& ctx)
    {
        yield();
        _file = SD.open(ctx.filename.c_str(), FILE_READ);
        if (!_file || !_file.available())
        {
            Serial.printf("Failed to open file for reading: %s\n", ctx.filename.c_str());
            return false;
        }

        Serial.printf("opened file for reading: %s\n", ctx.filename.c_str());

        return true;
    }

    bool openForWrite(IO_CONTEXT& ctx)
    {
        yield();
        _file = SD.open(ctx.filename.c_str(), FILE_WRITE_BEGIN);
        if (!_file)
        {
            Serial.printf("Failed to open file for writing: %s\n", ctx.filename.c_str());
            return false;
        }

        Serial.printf("opened file for writing: %s\n", ctx.filename.c_str());

        return true;
    }
    
    bool doneReading(IO_CONTEXT& ctx, void *buf)
    {
        Serial.printf("Reading file: %s\n", ctx.filename.c_str());

        uint32_t chunkSize = min(ASYNC_IO_R_BUFFER_SIZE, ctx.size - _readIO.total);

        yield();
        _readIO.total += _file.readBytes((char *)_readIO.index, chunkSize);
        _readIO.index += chunkSize;

        //Serial.printf("ctx.size: %d, _readIO.readTotal: %d, chunkSize: %d, _readIO.index: %d\n", ctx.size, _readIO.total, chunkSize, _readIO.index);

        if (_readIO.total >= ctx.size) {
            _file.close();

            Serial.printf("done reading file %s!\n", ctx.filename.c_str());

            return true;
        }

        return false;
    }

    bool doneWriting(IO_CONTEXT& ctx, const byte *buf)
    {
        Serial.printf("Writing file: %s ", ctx.filename.c_str());

        if (_writeIO.remaining > 0) {
            uint32_t chunkSize = min(ASYNC_IO_W_BUFFER_SIZE, _writeIO.remaining);

            yield();
            memcpy(_writeIO.buffer, (byte *)buf + _writeIO.offset, chunkSize);
            yield();
            uint32_t bytesWritten = _file.write(_writeIO.buffer, chunkSize);

            _writeIO.offset += chunkSize;
            _writeIO.remaining -= bytesWritten; // should use bytesWritten or chunkSize?

            //Serial.printf("REMAINING <=0, remaining: %d\n", _writeIO.remaining);

            if (_writeIO.remaining <= 0) {
                _file.close();

                Serial.printf("done writing file %s!\n", ctx.filename.c_str());

                _writeIO.remaining = 0;
                _writeIO.offset = 0;

                return true;
            }
        }

        return false;
    }

    void *getReadObject(FILE_TYPE fileType)
    {
        switch (fileType)
        {
            case PATTERN_SETTINGS:
                return (byte *)&XRSequencer::idlePatternSettings;
            case TRACK_LAYER:
                return (byte *)&XRSequencer::idleTrackLayer;
            case RATCHET_LAYER:
                return (byte *)&XRSequencer::idleRatchetLayer;
            case KIT:
                return (byte *)&XRSound::idleKit;
        }
    }

    byte *getWriteObject(FILE_TYPE fileType)
    {
        switch (fileType)
        {
            case PATTERN_SETTINGS:
                return (byte *)&XRSequencer::patternSettingsForWrite;
            case TRACK_LAYER:
                return (byte *)&XRSequencer::trackLayerForWrite;
            case RATCHET_LAYER:
                return (byte *)&XRSequencer::ratchetLayerForWrite;
            case KIT:
                return (byte *)&XRSound::kitForWrite;
        }
    }
}