static int decode_tdlt(GetByteContext *gb, uint8_t *frame, int width, int height)
{
    const uint8_t *frame_end = frame + width * height;
    uint32_t segments = bytestream2_get_le32(gb);
    int skip, copy;

    while (segments--) {
        if (bytestream2_get_bytes_left(gb) < 2)
            return AVERROR_INVALIDDATA;
        copy = bytestream2_get_byteu(gb) * 2;
        skip = bytestream2_get_byteu(gb) * 2;
        if (frame_end - frame < copy + skip ||
            bytestream2_get_bytes_left(gb) < copy)
            return AVERROR_INVALIDDATA;
        frame += skip;
        bytestream2_get_buffer(gb, frame, copy);
        frame += copy;
    }

    return 0;
}
