pkinit_init_req_crypto(pkinit_req_crypto_context *cryptoctx)
{
    krb5_error_code retval = ENOMEM;
    pkinit_req_crypto_context ctx = NULL;

    ctx = malloc(sizeof(*ctx));
    if (ctx == NULL)
        goto out;
    memset(ctx, 0, sizeof(*ctx));

    ctx->dh = NULL;
    ctx->received_cert = NULL;

    *cryptoctx = ctx;

    pkiDebug("%s: returning ctx at %p\n", __FUNCTION__, ctx);
    retval = 0;
out:
    if (retval)
        free(ctx);

    return retval;
}
