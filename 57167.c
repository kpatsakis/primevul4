void xchg_mb_border(uint8_t *top_border, uint8_t *src_y, uint8_t *src_cb,
                    uint8_t *src_cr, ptrdiff_t linesize, ptrdiff_t uvlinesize, int mb_x,
                    int mb_y, int mb_width, int simple, int xchg)
{
    uint8_t *top_border_m1 = top_border - 32;     // for TL prediction
    src_y  -= linesize;
    src_cb -= uvlinesize;
    src_cr -= uvlinesize;

#define XCHG(a, b, xchg)                                                      \
    do {                                                                      \
        if (xchg)                                                             \
            AV_SWAP64(b, a);                                                  \
        else                                                                  \
            AV_COPY64(b, a);                                                  \
    } while (0)

    XCHG(top_border_m1 + 8, src_y - 8, xchg);
    XCHG(top_border, src_y, xchg);
    XCHG(top_border + 8, src_y + 8, 1);
    if (mb_x < mb_width - 1)
        XCHG(top_border + 32, src_y + 16, 1);

    if (!simple || !mb_y) {
        XCHG(top_border_m1 + 16, src_cb - 8, xchg);
        XCHG(top_border_m1 + 24, src_cr - 8, xchg);
        XCHG(top_border + 16, src_cb, 1);
        XCHG(top_border + 24, src_cr, 1);
    }
}
