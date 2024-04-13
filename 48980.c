free_external_lucid_ctx_v1(
    gss_krb5_lucid_context_v1_t *ctx)
{
    if (ctx) {
        if (ctx->protocol == 0) {
            free_lucid_key_data(&ctx->rfc1964_kd.ctx_key);
        }
        if (ctx->protocol == 1) {
            free_lucid_key_data(&ctx->cfx_kd.ctx_key);
            if (ctx->cfx_kd.have_acceptor_subkey)
                free_lucid_key_data(&ctx->cfx_kd.acceptor_subkey);
        }
        xfree(ctx);
        ctx = NULL;
    }
}
