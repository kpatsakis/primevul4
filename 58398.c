static int do_encode(AVCodecContext *avctx, const AVFrame *frame, int *got_packet)
{
    int ret;
    *got_packet = 0;

    av_packet_unref(avctx->internal->buffer_pkt);
    avctx->internal->buffer_pkt_valid = 0;

    if (avctx->codec_type == AVMEDIA_TYPE_VIDEO) {
        ret = avcodec_encode_video2(avctx, avctx->internal->buffer_pkt,
                                    frame, got_packet);
    } else if (avctx->codec_type == AVMEDIA_TYPE_AUDIO) {
        ret = avcodec_encode_audio2(avctx, avctx->internal->buffer_pkt,
                                    frame, got_packet);
    } else {
        ret = AVERROR(EINVAL);
    }

    if (ret >= 0 && *got_packet) {
        av_assert0(!avctx->internal->buffer_pkt->data || avctx->internal->buffer_pkt->buf);
        avctx->internal->buffer_pkt_valid = 1;
        ret = 0;
    } else {
        av_packet_unref(avctx->internal->buffer_pkt);
    }

    return ret;
}
