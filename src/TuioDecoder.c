#include "TuioDecoder.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
    const uint8_t* data;
    uint32_t length;
    uint32_t offset;
} OscReader;

static uint32_t Pad4(uint32_t value)
{
    return (value + 3u) & ~3u;
}

static int ReaderRemaining(const OscReader* reader, uint32_t count)
{
    return reader->offset <= reader->length &&
           count <= (reader->length - reader->offset);
}

static int ReadInt32(OscReader* reader, int32_t* value)
{
    const uint8_t* p;

    if (!ReaderRemaining(reader, 4))
        return 0;

    p = reader->data + reader->offset;
    reader->offset += 4;

    *value = (int32_t)(((uint32_t)p[0] << 24) |
                       ((uint32_t)p[1] << 16) |
                       ((uint32_t)p[2] << 8) |
                       (uint32_t)p[3]);

    return 1;
}

static int ReadFloat32(OscReader* reader, float* value)
{
    int32_t raw;
    uint32_t bits;

    if (!ReadInt32(reader, &raw))
        return 0;

    bits = (uint32_t)raw;
    memcpy(value, &bits, sizeof(float));

    return 1;
}

static int ReadOscString(OscReader* reader, const char** value)
{
    uint32_t start;
    uint32_t end;
    uint32_t next;

    if (!ReaderRemaining(reader, 1))
        return 0;

    start = reader->offset;
    end = start;

    while (end < reader->length && reader->data[end] != 0)
        end++;

    if (end >= reader->length)
        return 0;

    next = Pad4(end + 1);
    if (next > reader->length)
        return 0;

    *value = (const char*)(reader->data + start);
    reader->offset = next;

    return 1;
}

static void CopyString(char* dst, uint32_t dstLen, const char* src)
{
    if (dstLen == 0)
        return;

    strncpy(dst, src, dstLen - 1);
    dst[dstLen - 1] = 0;
}

static int IsTuio2DCurAddress(const char* address)
{
    return 0 == strcmp(address, "/tuio/2Dcur");
}

static int AddAliveId(TuioFrame* frame, int sessionId)
{
    if (frame->aliveCount >= TUIO_MAX_TOUCHES)
        return 0;

    frame->aliveIds[frame->aliveCount++] = sessionId;
    return 1;
}

static int AddTouch(TuioFrame* frame,
                    int sessionId,
                    float x,
                    float y,
                    float dx,
                    float dy,
                    float motionAccel)
{
    TuioTouch* touch;

    if (frame->touchCount >= TUIO_MAX_TOUCHES)
        return 0;

    touch = &frame->touches[frame->touchCount++];
    touch->sessionId = sessionId;
    touch->x = x;
    touch->y = y;
    touch->dx = dx;
    touch->dy = dy;
    touch->motionAccel = motionAccel;

    return 1;
}

static int ParseTuioMessage(const uint8_t* packet,
                            uint32_t length,
                            TuioFrame* frame)
{
    OscReader reader;
    const char* address;
    const char* types;
    const char* command;
    uint32_t argIndex;

    reader.data = packet;
    reader.length = length;
    reader.offset = 0;

    if (!ReadOscString(&reader, &address) ||
        !ReadOscString(&reader, &types))
        return 0;

    if (!IsTuio2DCurAddress(address) || types[0] != ',' || types[1] != 's')
        return 0;

    if (!ReadOscString(&reader, &command))
        return 0;

    argIndex = 2;

    if (0 == strcmp(command, "source")) {
        const char* source;

        if (types[argIndex++] != 's' ||
            !ReadOscString(&reader, &source))
            return 0;

        CopyString(frame->source, TUIO_MAX_SOURCE_LEN, source);
        return 1;
    }

    if (0 == strcmp(command, "alive")) {
        frame->aliveCount = 0;

        while (types[argIndex] != 0) {
            int32_t sessionId;

            if (types[argIndex++] != 'i' ||
                !ReadInt32(&reader, &sessionId))
                return 0;

            (void)AddAliveId(frame, (int)sessionId);
        }

        return 1;
    }

    if (0 == strcmp(command, "set")) {
        int32_t sessionId;
        float x;
        float y;
        float dx;
        float dy;
        float motionAccel;

        if (0 != strcmp(&types[argIndex], "ifffff"))
            return 0;

        if (!ReadInt32(&reader, &sessionId) ||
            !ReadFloat32(&reader, &x) ||
            !ReadFloat32(&reader, &y) ||
            !ReadFloat32(&reader, &dx) ||
            !ReadFloat32(&reader, &dy) ||
            !ReadFloat32(&reader, &motionAccel))
            return 0;

        (void)AddTouch(frame,
                       (int)sessionId,
                       x,
                       y,
                       dx,
                       dy,
                       motionAccel);
        return 1;
    }

    if (0 == strcmp(command, "fseq")) {
        int32_t frameId;

        if (types[argIndex] != 'i' ||
            !ReadInt32(&reader, &frameId))
            return 0;

        frame->frame = (int)frameId;
        return 1;
    }

    return 0;
}

static int ParseOscPacket(const uint8_t* packet,
                          uint32_t length,
                          TuioFrame* frame,
                          int depth)
{
    OscReader reader;
    int decoded = 0;

    if (depth > 8 || length == 0)
        return 0;

    if (length >= 16 && 0 == memcmp(packet, "#bundle", 8)) {
        reader.data = packet;
        reader.length = length;
        reader.offset = 16; /* "#bundle\0" plus OSC timetag. */

        while (ReaderRemaining(&reader, 4)) {
            int32_t elementSize;

            if (!ReadInt32(&reader, &elementSize) || elementSize < 0)
                return decoded;

            if (!ReaderRemaining(&reader, (uint32_t)elementSize))
                return decoded;

            decoded |= ParseOscPacket(reader.data + reader.offset,
                                      (uint32_t)elementSize,
                                      frame,
                                      depth + 1);
            reader.offset += (uint32_t)elementSize;
        }

        return decoded;
    }

    return ParseTuioMessage(packet, length, frame);
}

static void Append(char* buffer, int bufferSize, int* offset, const char* format, ...)
{
    int written;
    va_list args;

    if (bufferSize <= 0 || *offset >= bufferSize)
        return;

    va_start(args, format);
    written = vsnprintf(buffer + *offset,
                        (size_t)(bufferSize - *offset),
                        format,
                        args);
    va_end(args);

    if (written < 0)
        return;

    if (written >= bufferSize - *offset)
        *offset = bufferSize - 1;
    else
        *offset += written;
}

void TuioFrameInit(TuioFrame* frame)
{
    memset(frame, 0, sizeof(TuioFrame));
}

int TuioDecodePacket(const uint8_t* packet,
                     uint32_t length,
                     TuioFrame* frame)
{
    if (NULL == packet || NULL == frame)
        return 0;

    TuioFrameInit(frame);

    return ParseOscPacket(packet, length, frame, 0);
}

void TuioFrameToString(const TuioFrame* frame,
                       char* buffer,
                       int bufferSize)
{
    int offset = 0;
    int i;

    if (NULL == frame || NULL == buffer || bufferSize <= 0)
        return;

    buffer[0] = 0;

    Append(buffer,
           bufferSize,
           &offset,
           "source=%s frame=%d alive=%d touches=%d",
           frame->source[0] ? frame->source : "(none)",
           frame->frame,
           frame->aliveCount,
           frame->touchCount);

    if (frame->aliveCount > 0) {
        Append(buffer, bufferSize, &offset, " aliveIds=[");
        for (i = 0; i < frame->aliveCount; i++) {
            Append(buffer,
                   bufferSize,
                   &offset,
                   "%s%d",
                   (i == 0) ? "" : ",",
                   frame->aliveIds[i]);
        }
        Append(buffer, bufferSize, &offset, "]");
    }

    if (frame->touchCount > 0) {
        Append(buffer, bufferSize, &offset, " touches=[");
        for (i = 0; i < frame->touchCount; i++) {
            const TuioTouch* touch = &frame->touches[i];

            Append(buffer,
                   bufferSize,
                   &offset,
                   "%s{id=%d x=%.4f y=%.4f dx=%.4f dy=%.4f m=%.4f}",
                   (i == 0) ? "" : ",",
                   touch->sessionId,
                   touch->x,
                   touch->y,
                   touch->dx,
                   touch->dy,
                   touch->motionAccel);
        }
        Append(buffer, bufferSize, &offset, "]");
    }
}
