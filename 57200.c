static int vp8_lossy_decode_alpha(AVCodecContext *avctx, AVFrame *p,
                                  uint8_t *data_start,
                                  unsigned int data_size)
{
    WebPContext *s = avctx->priv_data;
    int x, y, ret;

    if (s->alpha_compression == ALPHA_COMPRESSION_NONE) {
        GetByteContext gb;

        bytestream2_init(&gb, data_start, data_size);
        for (y = 0; y < s->height; y++)
            bytestream2_get_buffer(&gb, p->data[3] + p->linesize[3] * y,
                                   s->width);
    } else if (s->alpha_compression == ALPHA_COMPRESSION_VP8L) {
        uint8_t *ap, *pp;
        int alpha_got_frame = 0;

        s->alpha_frame = av_frame_alloc();
        if (!s->alpha_frame)
            return AVERROR(ENOMEM);

        ret = vp8_lossless_decode_frame(avctx, s->alpha_frame, &alpha_got_frame,
                                        data_start, data_size, 1);
        if (ret < 0) {
            av_frame_free(&s->alpha_frame);
            return ret;
        }
        if (!alpha_got_frame) {
            av_frame_free(&s->alpha_frame);
            return AVERROR_INVALIDDATA;
        }

        /* copy green component of alpha image to alpha plane of primary image */
        for (y = 0; y < s->height; y++) {
            ap = GET_PIXEL(s->alpha_frame, 0, y) + 2;
            pp = p->data[3] + p->linesize[3] * y;
            for (x = 0; x < s->width; x++) {
                *pp = *ap;
                pp++;
                ap += 4;
            }
        }
        av_frame_free(&s->alpha_frame);
    }

    /* apply alpha filtering */
    if (s->alpha_filter)
        alpha_inverse_prediction(p, s->alpha_filter);

    return 0;
}
