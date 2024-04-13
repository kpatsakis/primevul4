static int decode_copy(GetByteContext *gb, uint8_t *frame, int width, int height)
{
    const int size = width * height;

    if (bytestream2_get_buffer(gb, frame, size) != size)
        return AVERROR_INVALIDDATA;
    return 0;
}
