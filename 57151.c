static int vp8_alloc_frame(VP8Context *s, VP8Frame *f, int ref)
{
    int ret;
    if ((ret = ff_thread_get_buffer(s->avctx, &f->tf,
                                    ref ? AV_GET_BUFFER_FLAG_REF : 0)) < 0)
        return ret;
    if (!(f->seg_map = av_buffer_allocz(s->mb_width * s->mb_height))) {
        ff_thread_release_buffer(s->avctx, &f->tf);
        return AVERROR(ENOMEM);
    }
    return 0;
}
