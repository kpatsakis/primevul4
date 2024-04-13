void X509_STORE_CTX_set0_param(X509_STORE_CTX *ctx, X509_VERIFY_PARAM *param)
{
    X509_VERIFY_PARAM_free(ctx->param);
    ctx->param = param;
}
