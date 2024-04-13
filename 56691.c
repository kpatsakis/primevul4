static int dnxhd_init_vlc(DNXHDContext *ctx, uint32_t cid, int bitdepth)
{
    if (cid != ctx->cid) {
        int index;

        if ((index = ff_dnxhd_get_cid_table(cid)) < 0) {
            av_log(ctx->avctx, AV_LOG_ERROR, "unsupported cid %"PRIu32"\n", cid);
            return AVERROR(ENOSYS);
        }
        if (ff_dnxhd_cid_table[index].bit_depth != bitdepth &&
            ff_dnxhd_cid_table[index].bit_depth != DNXHD_VARIABLE) {
            av_log(ctx->avctx, AV_LOG_ERROR, "bit depth mismatches %d %d\n", ff_dnxhd_cid_table[index].bit_depth, bitdepth);
            return AVERROR_INVALIDDATA;
        }
        ctx->cid_table = &ff_dnxhd_cid_table[index];
        av_log(ctx->avctx, AV_LOG_VERBOSE, "Profile cid %"PRIu32".\n", cid);

        ff_free_vlc(&ctx->ac_vlc);
        ff_free_vlc(&ctx->dc_vlc);
        ff_free_vlc(&ctx->run_vlc);

        init_vlc(&ctx->ac_vlc, DNXHD_VLC_BITS, 257,
                 ctx->cid_table->ac_bits, 1, 1,
                 ctx->cid_table->ac_codes, 2, 2, 0);
        init_vlc(&ctx->dc_vlc, DNXHD_DC_VLC_BITS, bitdepth > 8 ? 14 : 12,
                 ctx->cid_table->dc_bits, 1, 1,
                 ctx->cid_table->dc_codes, 1, 1, 0);
        init_vlc(&ctx->run_vlc, DNXHD_VLC_BITS, 62,
                 ctx->cid_table->run_bits, 1, 1,
                 ctx->cid_table->run_codes, 2, 2, 0);

        ctx->cid = cid;
    }
    return 0;
}
