iakerb_release_context(iakerb_ctx_id_t ctx)
{
    OM_uint32 tmp;

    if (ctx == NULL)
        return;

    krb5_gss_release_cred(&tmp, &ctx->defcred);
    krb5_init_creds_free(ctx->k5c, ctx->icc);
    krb5_tkt_creds_free(ctx->k5c, ctx->tcc);
    krb5_gss_delete_sec_context(&tmp, &ctx->gssc, NULL);
    krb5_free_data_contents(ctx->k5c, &ctx->conv);
    krb5_get_init_creds_opt_free(ctx->k5c, ctx->gic_opts);
    krb5_free_context(ctx->k5c);
    free(ctx);
}
