static av_cold int decode_end(AVCodecContext *avctx)
{
    SmackVContext * const smk = avctx->priv_data;

    av_freep(&smk->mmap_tbl);
    av_freep(&smk->mclr_tbl);
    av_freep(&smk->full_tbl);
    av_freep(&smk->type_tbl);

    av_frame_free(&smk->pic);

    return 0;
}
