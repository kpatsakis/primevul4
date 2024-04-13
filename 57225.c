static int decode_bdlt(GetByteContext *gb, uint8_t *frame, int width, int height)
{
    uint8_t *line_ptr;
    int count, lines, segments;

    count = bytestream2_get_le16(gb);
    if (count >= height)
        return AVERROR_INVALIDDATA;
    frame += width * count;
    lines = bytestream2_get_le16(gb);
    if (count + lines > height)
        return AVERROR_INVALIDDATA;

    while (lines--) {
        if (bytestream2_get_bytes_left(gb) < 1)
            return AVERROR_INVALIDDATA;
        line_ptr = frame;
        frame += width;
        segments = bytestream2_get_byteu(gb);
        while (segments--) {
            if (frame - line_ptr <= bytestream2_peek_byte(gb))
                return AVERROR_INVALIDDATA;
            line_ptr += bytestream2_get_byte(gb);
            count = (int8_t)bytestream2_get_byte(gb);
            if (count >= 0) {
                if (frame - line_ptr < count)
                    return AVERROR_INVALIDDATA;
                if (bytestream2_get_buffer(gb, line_ptr, count) != count)
                    return AVERROR_INVALIDDATA;
            } else {
                count = -count;
                if (frame - line_ptr < count)
                    return AVERROR_INVALIDDATA;
                memset(line_ptr, bytestream2_get_byte(gb), count);
            }
            line_ptr += count;
        }
    }

    return 0;
}
