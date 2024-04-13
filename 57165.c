static void vp8_release_frame(VP8Context *s, VP8Frame *f)
{
    av_buffer_unref(&f->seg_map);
    ff_thread_release_buffer(s->avctx, &f->tf);
}
