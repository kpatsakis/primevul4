static void do_init_filter(APEFilter *f, int16_t *buf, int order)
{
    f->coeffs = buf;
    f->historybuffer = buf + order;
    f->delay       = f->historybuffer + order * 2;
    f->adaptcoeffs = f->historybuffer + order;

    memset(f->historybuffer, 0, (order * 2) * sizeof(*f->historybuffer));
    memset(f->coeffs, 0, order * sizeof(*f->coeffs));
    f->avg = 0;
}
