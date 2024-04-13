static int vp7_decode_mb_row_no_filter(AVCodecContext *avctx, void *tdata,
                                        int jobnr, int threadnr)
{
    return decode_mb_row_no_filter(avctx, tdata, jobnr, threadnr, 1);
}
