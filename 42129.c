pkinit_init_identity_crypto(pkinit_identity_crypto_context *idctx)
{
    krb5_error_code retval = ENOMEM;
    pkinit_identity_crypto_context ctx = NULL;

    ctx = malloc(sizeof(*ctx));
    if (ctx == NULL)
        goto out;
    memset(ctx, 0, sizeof(*ctx));

    ctx->identity = NULL;

    retval = pkinit_init_certs(ctx);
    if (retval)
        goto out;

    retval = pkinit_init_pkcs11(ctx);
    if (retval)
        goto out;

    pkiDebug("%s: returning ctx at %p\n", __FUNCTION__, ctx);
    *idctx = ctx;

out:
    if (retval) {
        if (ctx)
            pkinit_fini_identity_crypto(ctx);
    }

    return retval;
}
