static int vp7_decode_mb_row_sliced(AVCodecContext *avctx, void *tdata,
                                    int jobnr, int threadnr)
{
    return vp78_decode_mb_row_sliced(avctx, tdata, jobnr, threadnr, IS_VP7);
}
