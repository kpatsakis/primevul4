pkinit_init_plg_crypto(pkinit_plg_crypto_context *cryptoctx)
{
    krb5_error_code retval = ENOMEM;
    pkinit_plg_crypto_context ctx = NULL;

    /* initialize openssl routines */
    openssl_init();

    ctx = malloc(sizeof(*ctx));
    if (ctx == NULL)
        goto out;
    memset(ctx, 0, sizeof(*ctx));

    pkiDebug("%s: initializing openssl crypto context at %p\n",
             __FUNCTION__, ctx);
    retval = pkinit_init_pkinit_oids(ctx);
    if (retval)
        goto out;

    retval = pkinit_init_dh_params(ctx);
    if (retval)
        goto out;

    *cryptoctx = ctx;

out:
    if (retval && ctx != NULL)
        pkinit_fini_plg_crypto(ctx);

    return retval;
}
