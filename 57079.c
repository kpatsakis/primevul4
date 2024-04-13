static void bitplanar2chunky(CDXLVideoContext *c, int linesize, uint8_t *out)
{
    GetBitContext gb;
    int x, y, plane;

    if (init_get_bits8(&gb, c->video, c->video_size) < 0)
        return;
    for (plane = 0; plane < c->bpp; plane++) {
        for (y = 0; y < c->avctx->height; y++) {
            for (x = 0; x < c->avctx->width; x++)
                out[linesize * y + x] |= get_bits1(&gb) << plane;
            skip_bits(&gb, c->padded_bits);
        }
    }
}
