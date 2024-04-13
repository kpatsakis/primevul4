static void init_filter(APEContext *ctx, APEFilter *f, int16_t *buf, int order)
{
    do_init_filter(&f[0], buf, order);
    do_init_filter(&f[1], buf + order * 3 + HISTORY_SIZE, order);
}
