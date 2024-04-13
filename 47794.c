void X509_STORE_CTX_cleanup(X509_STORE_CTX *ctx)
{
    if (ctx->cleanup)
        ctx->cleanup(ctx);
    if (ctx->param != NULL) {
        if (ctx->parent == NULL)
            X509_VERIFY_PARAM_free(ctx->param);
        ctx->param = NULL;
    }
    X509_policy_tree_free(ctx->tree);
    ctx->tree = NULL;
    sk_X509_pop_free(ctx->chain, X509_free);
    ctx->chain = NULL;
    CRYPTO_free_ex_data(CRYPTO_EX_INDEX_X509_STORE_CTX, ctx, &(ctx->ex_data));
    memset(&ctx->ex_data, 0, sizeof(ctx->ex_data));
}
