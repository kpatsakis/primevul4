static void fade(uint8_t *dst, ptrdiff_t dst_linesize,
                 const uint8_t *src, ptrdiff_t src_linesize,
                 int width, int height,
                 int alpha, int beta)
{
    int i, j;
    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            uint8_t y = src[j * src_linesize + i];
            dst[j * dst_linesize + i] = av_clip_uint8(y + ((y * beta) >> 8) + alpha);
        }
    }
}
