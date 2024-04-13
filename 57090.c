static int decode_frame(AVCodecContext *avctx, void *data, int *got_frame,
                        AVPacket *avpkt)
{
    SCPRContext *s = avctx->priv_data;
    GetByteContext *gb = &s->gb;
    AVFrame *frame = data;
    int ret, type;

    if (avctx->bits_per_coded_sample == 16) {
        if ((ret = ff_get_buffer(avctx, frame, 0)) < 0)
            return ret;
    }

    if ((ret = ff_reget_buffer(avctx, s->current_frame)) < 0)
        return ret;

    bytestream2_init(gb, avpkt->data, avpkt->size);

    type = bytestream2_peek_byte(gb);

    if (type == 2) {
        s->get_freq = get_freq0;
        s->decode = decode0;
        frame->key_frame = 1;
        ret = decompress_i(avctx, (uint32_t *)s->current_frame->data[0],
                           s->current_frame->linesize[0] / 4);
    } else if (type == 18) {
        s->get_freq = get_freq;
        s->decode = decode;
        frame->key_frame = 1;
        ret = decompress_i(avctx, (uint32_t *)s->current_frame->data[0],
                           s->current_frame->linesize[0] / 4);
    } else if (type == 17) {
        uint32_t clr, *dst = (uint32_t *)s->current_frame->data[0];
        int x, y;

        frame->key_frame = 1;
        bytestream2_skip(gb, 1);
        if (avctx->bits_per_coded_sample == 16) {
            uint16_t value = bytestream2_get_le16(gb);
            int r, g, b;

            r = (value      ) & 31;
            g = (value >>  5) & 31;
            b = (value >> 10) & 31;
            clr = (r << 16) + (g << 8) + b;
        } else {
            clr = bytestream2_get_le24(gb);
        }
        for (y = 0; y < avctx->height; y++) {
            for (x = 0; x < avctx->width; x++) {
                dst[x] = clr;
            }
            dst += s->current_frame->linesize[0] / 4;
        }
    } else if (type == 0 || type == 1) {
        frame->key_frame = 0;

        ret = av_frame_copy(s->current_frame, s->last_frame);
        if (ret < 0)
            return ret;

        ret = decompress_p(avctx, (uint32_t *)s->current_frame->data[0],
                           s->current_frame->linesize[0] / 4,
                           (uint32_t *)s->last_frame->data[0],
                           s->last_frame->linesize[0] / 4);
    } else {
        return AVERROR_PATCHWELCOME;
    }

    if (ret < 0)
        return ret;

    if (avctx->bits_per_coded_sample != 16) {
        ret = av_frame_ref(data, s->current_frame);
        if (ret < 0)
            return ret;
    } else {
        uint8_t *dst = frame->data[0];
        int x, y;

        ret = av_frame_copy(frame, s->current_frame);
        if (ret < 0)
            return ret;

        for (y = 0; y < avctx->height; y++) {
            for (x = 0; x < avctx->width * 4; x++) {
                dst[x] = dst[x] << 3;
            }
            dst += frame->linesize[0];
        }
    }

    frame->pict_type = frame->key_frame ? AV_PICTURE_TYPE_I : AV_PICTURE_TYPE_P;

    FFSWAP(AVFrame *, s->current_frame, s->last_frame);

    frame->data[0]     += frame->linesize[0] * (avctx->height - 1);
    frame->linesize[0] *= -1;

    *got_frame = 1;

    return avpkt->size;
}
