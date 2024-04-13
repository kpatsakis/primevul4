void avcodec_flush_buffers(AVCodecContext *avctx)
{
    avctx->internal->draining      = 0;
    avctx->internal->draining_done = 0;
    av_frame_unref(avctx->internal->buffer_frame);
    av_packet_unref(avctx->internal->buffer_pkt);
    avctx->internal->buffer_pkt_valid = 0;

    if (HAVE_THREADS && avctx->active_thread_type & FF_THREAD_FRAME)
        ff_thread_flush(avctx);
    else if (avctx->codec->flush)
        avctx->codec->flush(avctx);

    avctx->pts_correction_last_pts =
    avctx->pts_correction_last_dts = INT64_MIN;

    if (!avctx->refcounted_frames)
        av_frame_unref(avctx->internal->to_free);
}
