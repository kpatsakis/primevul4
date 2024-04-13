void prefetch_motion(VP8Context *s, VP8Macroblock *mb, int mb_x, int mb_y,
                     int mb_xy, int ref)
{
    /* Don't prefetch refs that haven't been used very often this frame. */
    if (s->ref_count[ref - 1] > (mb_xy >> 5)) {
        int x_off = mb_x << 4, y_off = mb_y << 4;
        int mx = (mb->mv.x >> 2) + x_off + 8;
        int my = (mb->mv.y >> 2) + y_off;
        uint8_t **src = s->framep[ref]->tf.f->data;
        int off = mx + (my + (mb_x & 3) * 4) * s->linesize + 64;
        /* For threading, a ff_thread_await_progress here might be useful, but
         * it actually slows down the decoder. Since a bad prefetch doesn't
         * generate bad decoder output, we don't run it here. */
        s->vdsp.prefetch(src[0] + off, s->linesize, 4);
        off = (mx >> 1) + ((my >> 1) + (mb_x & 7)) * s->uvlinesize + 64;
        s->vdsp.prefetch(src[1] + off, src[2] - src[1], 2);
    }
}
