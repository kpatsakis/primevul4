void backup_mb_border(uint8_t *top_border, uint8_t *src_y,
                      uint8_t *src_cb, uint8_t *src_cr,
                      ptrdiff_t linesize, ptrdiff_t uvlinesize, int simple)
{
    AV_COPY128(top_border, src_y + 15 * linesize);
    if (!simple) {
        AV_COPY64(top_border + 16, src_cb + 7 * uvlinesize);
        AV_COPY64(top_border + 24, src_cr + 7 * uvlinesize);
    }
}
