static void chunky2chunky(CDXLVideoContext *c, int linesize, uint8_t *out)
{
    GetByteContext gb;
    int y;

    bytestream2_init(&gb, c->video, c->video_size);
    for (y = 0; y < c->avctx->height; y++) {
        bytestream2_get_buffer(&gb, out + linesize * y, c->avctx->width * 3);
    }
}
