static int dfa_decode_frame(AVCodecContext *avctx,
                            void *data, int *got_frame,
                            AVPacket *avpkt)
{
    AVFrame *frame = data;
    DfaContext *s = avctx->priv_data;
    GetByteContext gb;
    const uint8_t *buf = avpkt->data;
    uint32_t chunk_type, chunk_size;
    uint8_t *dst;
    int ret;
    int i, pal_elems;
    int version = avctx->extradata_size==2 ? AV_RL16(avctx->extradata) : 0;

    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0)
        return ret;

    bytestream2_init(&gb, avpkt->data, avpkt->size);
    while (bytestream2_get_bytes_left(&gb) > 0) {
        bytestream2_skip(&gb, 4);
        chunk_size = bytestream2_get_le32(&gb);
        chunk_type = bytestream2_get_le32(&gb);
        if (!chunk_type)
            break;
        if (chunk_type == 1) {
            pal_elems = FFMIN(chunk_size / 3, 256);
            for (i = 0; i < pal_elems; i++) {
                s->pal[i] = bytestream2_get_be24(&gb) << 2;
                s->pal[i] |= 0xFFU << 24 | (s->pal[i] >> 6) & 0x30303;
            }
            frame->palette_has_changed = 1;
        } else if (chunk_type <= 9) {
            if (decoder[chunk_type - 2](&gb, s->frame_buf, avctx->width, avctx->height)) {
                av_log(avctx, AV_LOG_ERROR, "Error decoding %s chunk\n",
                       chunk_name[chunk_type - 2]);
                return AVERROR_INVALIDDATA;
            }
        } else {
            av_log(avctx, AV_LOG_WARNING,
                   "Ignoring unknown chunk type %"PRIu32"\n",
                   chunk_type);
        }
        buf += chunk_size;
    }

    buf = s->frame_buf;
    dst = frame->data[0];
    for (i = 0; i < avctx->height; i++) {
        if(version == 0x100) {
            int j;
            for(j = 0; j < avctx->width; j++) {
                dst[j] = buf[ (i&3)*(avctx->width /4) + (j/4) +
                             ((j&3)*(avctx->height/4) + (i/4))*avctx->width];
            }
        } else {
            memcpy(dst, buf, avctx->width);
            buf += avctx->width;
        }
        dst += frame->linesize[0];
    }
    memcpy(frame->data[1], s->pal, sizeof(s->pal));

    *got_frame = 1;

    return avpkt->size;
}
