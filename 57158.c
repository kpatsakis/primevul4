static int vp8_decode_update_thread_context(AVCodecContext *dst,
                                            const AVCodecContext *src)
{
    VP8Context *s = dst->priv_data, *s_src = src->priv_data;
    int i;

    if (s->macroblocks_base &&
        (s_src->mb_width != s->mb_width || s_src->mb_height != s->mb_height)) {
        free_buffers(s);
        s->mb_width  = s_src->mb_width;
        s->mb_height = s_src->mb_height;
    }

    s->prob[0]      = s_src->prob[!s_src->update_probabilities];
    s->segmentation = s_src->segmentation;
    s->lf_delta     = s_src->lf_delta;
    memcpy(s->sign_bias, s_src->sign_bias, sizeof(s->sign_bias));

    for (i = 0; i < FF_ARRAY_ELEMS(s_src->frames); i++) {
        if (s_src->frames[i].tf.f->data[0]) {
            int ret = vp8_ref_frame(s, &s->frames[i], &s_src->frames[i]);
            if (ret < 0)
                return ret;
        }
    }

    s->framep[0] = REBASE(s_src->next_framep[0]);
    s->framep[1] = REBASE(s_src->next_framep[1]);
    s->framep[2] = REBASE(s_src->next_framep[2]);
    s->framep[3] = REBASE(s_src->next_framep[3]);

    return 0;
}
