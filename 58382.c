int attribute_align_arg avcodec_receive_frame(AVCodecContext *avctx, AVFrame *frame)
{
    int ret;

    av_frame_unref(frame);

    if (!avcodec_is_open(avctx) || !av_codec_is_decoder(avctx->codec))
        return AVERROR(EINVAL);

    if (avctx->codec->receive_frame) {
        if (avctx->internal->draining && !(avctx->codec->capabilities & AV_CODEC_CAP_DELAY))
            return AVERROR_EOF;
        ret = avctx->codec->receive_frame(avctx, frame);
        if (ret >= 0) {
            if (av_frame_get_best_effort_timestamp(frame) == AV_NOPTS_VALUE) {
                av_frame_set_best_effort_timestamp(frame,
                    guess_correct_pts(avctx, frame->pts, frame->pkt_dts));
            }
        }
        return ret;
    }


    if (!avctx->internal->buffer_frame->buf[0]) {
        if (!avctx->internal->buffer_pkt->size && !avctx->internal->draining)
            return AVERROR(EAGAIN);

        while (1) {
            if ((ret = do_decode(avctx, avctx->internal->buffer_pkt)) < 0) {
                av_packet_unref(avctx->internal->buffer_pkt);
                return ret;
            }
            if (avctx->internal->buffer_frame->buf[0] ||
                !avctx->internal->buffer_pkt->size)
                break;
        }
    }

    if (!avctx->internal->buffer_frame->buf[0])
        return avctx->internal->draining ? AVERROR_EOF : AVERROR(EAGAIN);

    av_frame_move_ref(frame, avctx->internal->buffer_frame);
    return 0;
}
