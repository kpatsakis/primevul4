static int get_bit_length(H264Context *h, const uint8_t *buf,
                          const uint8_t *ptr, int dst_length,
                          int i, int next_avc)
{
    if ((h->workaround_bugs & FF_BUG_AUTODETECT) && i + 3 < next_avc &&
        buf[i]     == 0x00 && buf[i + 1] == 0x00 &&
        buf[i + 2] == 0x01 && buf[i + 3] == 0xE0)
        h->workaround_bugs |= FF_BUG_TRUNCATED;

    if (!(h->workaround_bugs & FF_BUG_TRUNCATED))
        while (dst_length > 0 && ptr[dst_length - 1] == 0)
            dst_length--;

    if (!dst_length)
        return 0;

    return 8 * dst_length - decode_rbsp_trailing(h, ptr + dst_length - 1);
}
