void ff_thread_release_buffer(AVCodecContext *avctx, ThreadFrame *f)
{
    if (f->f)
        av_frame_unref(f->f);
}
