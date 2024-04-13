static void vp8_filter_mb_row(AVCodecContext *avctx, void *tdata,
                              int jobnr, int threadnr)
{
    filter_mb_row(avctx, tdata, jobnr, threadnr, 0);
}
