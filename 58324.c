static void handle_p_frame_png(PNGDecContext *s, AVFrame *p)
{
    int i, j;
    uint8_t *pd      = p->data[0];
    uint8_t *pd_last = s->last_picture.f->data[0];
    int ls = FFMIN(av_image_get_linesize(p->format, s->width, 0), s->width * s->bpp);

    ff_thread_await_progress(&s->last_picture, INT_MAX, 0);
    for (j = 0; j < s->height; j++) {
        for (i = 0; i < ls; i++)
            pd[i] += pd_last[i];
        pd      += s->image_linesize;
        pd_last += s->image_linesize;
    }
}
