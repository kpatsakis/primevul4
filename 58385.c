int attribute_align_arg avcodec_send_packet(AVCodecContext *avctx, const AVPacket *avpkt)
{
    int ret;

    if (!avcodec_is_open(avctx) || !av_codec_is_decoder(avctx->codec))
        return AVERROR(EINVAL);

    if (avctx->internal->draining)
        return AVERROR_EOF;

    if (avpkt && !avpkt->size && avpkt->data)
        return AVERROR(EINVAL);

    if (!avpkt || !avpkt->size) {
        avctx->internal->draining = 1;
        avpkt = NULL;

        if (!(avctx->codec->capabilities & AV_CODEC_CAP_DELAY))
            return 0;
    }

    if (avctx->codec->send_packet) {
        if (avpkt) {
            AVPacket tmp = *avpkt;
            int did_split = av_packet_split_side_data(&tmp);
            ret = apply_param_change(avctx, &tmp);
            if (ret >= 0)
                ret = avctx->codec->send_packet(avctx, &tmp);
            if (did_split)
                av_packet_free_side_data(&tmp);
            return ret;
        } else {
            return avctx->codec->send_packet(avctx, NULL);
        }
    }


    if (avctx->internal->buffer_pkt->size || avctx->internal->buffer_frame->buf[0])
        return AVERROR(EAGAIN);

    return do_decode(avctx, (AVPacket *)avpkt);
}
