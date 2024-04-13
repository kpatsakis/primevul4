int attribute_align_arg avcodec_receive_packet(AVCodecContext *avctx, AVPacket *avpkt)
{
    av_packet_unref(avpkt);

    if (!avcodec_is_open(avctx) || !av_codec_is_encoder(avctx->codec))
        return AVERROR(EINVAL);

    if (avctx->codec->receive_packet) {
        if (avctx->internal->draining && !(avctx->codec->capabilities & AV_CODEC_CAP_DELAY))
            return AVERROR_EOF;
        return avctx->codec->receive_packet(avctx, avpkt);
    }


    if (!avctx->internal->buffer_pkt_valid) {
        int got_packet;
        int ret;
        if (!avctx->internal->draining)
            return AVERROR(EAGAIN);
        ret = do_encode(avctx, NULL, &got_packet);
        if (ret < 0)
            return ret;
        if (ret >= 0 && !got_packet)
            return AVERROR_EOF;
    }

    av_packet_move_ref(avpkt, avctx->internal->buffer_pkt);
    avctx->internal->buffer_pkt_valid = 0;
    return 0;
}
