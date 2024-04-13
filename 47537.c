iakerb_make_exts(iakerb_ctx_id_t ctx, krb5_gss_ctx_ext_rec *exts)
{
    memset(exts, 0, sizeof(*exts));

    if (ctx->conv.length != 0)
        exts->iakerb.conv = &ctx->conv;
}
