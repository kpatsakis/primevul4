static void picmemset_8bpp(PicContext *s, AVFrame *frame, int value, int run,
                           int *x, int *y)
{
    while (run > 0) {
        uint8_t *d = frame->data[0] + *y * frame->linesize[0];
        if (*x + run >= s->width) {
            int n = s->width - *x;
            memset(d + *x, value, n);
            run -= n;
            *x = 0;
            *y -= 1;
            if (*y < 0)
                break;
        } else {
            memset(d + *x, value, run);
            *x += run;
            break;
        }
    }
}
