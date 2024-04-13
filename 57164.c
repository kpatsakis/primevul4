static int vp8_ref_frame(VP8Context *s, VP8Frame *dst, VP8Frame *src)
{
    int ret;

    vp8_release_frame(s, dst);

    if ((ret = ff_thread_ref_frame(&dst->tf, &src->tf)) < 0)
        return ret;
    if (src->seg_map &&
        !(dst->seg_map = av_buffer_ref(src->seg_map))) {
        vp8_release_frame(s, dst);
        return AVERROR(ENOMEM);
    }

    return 0;
}
