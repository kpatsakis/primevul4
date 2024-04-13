pkinit_server_plugin_init_realm(krb5_context context, const char *realmname,
                                pkinit_kdc_context *pplgctx)
{
    krb5_error_code retval = ENOMEM;
    pkinit_kdc_context plgctx = NULL;

    *pplgctx = NULL;

    plgctx = calloc(1, sizeof(*plgctx));
    if (plgctx == NULL)
        goto errout;

    pkiDebug("%s: initializing context at %p for realm '%s'\n",
             __FUNCTION__, plgctx, realmname);
    memset(plgctx, 0, sizeof(*plgctx));
    plgctx->magic = PKINIT_CTX_MAGIC;

    plgctx->realmname = strdup(realmname);
    if (plgctx->realmname == NULL)
        goto errout;
    plgctx->realmname_len = strlen(plgctx->realmname);

    retval = pkinit_init_plg_crypto(&plgctx->cryptoctx);
    if (retval)
        goto errout;

    retval = pkinit_init_plg_opts(&plgctx->opts);
    if (retval)
        goto errout;

    retval = pkinit_init_identity_crypto(&plgctx->idctx);
    if (retval)
        goto errout;

    retval = pkinit_init_identity_opts(&plgctx->idopts);
    if (retval)
        goto errout;

    retval = pkinit_init_kdc_profile(context, plgctx);
    if (retval)
        goto errout;

    retval = pkinit_identity_initialize(context, plgctx->cryptoctx, NULL,
                                        plgctx->idopts, plgctx->idctx, 0, NULL);
    if (retval)
        goto errout;

    pkiDebug("%s: returning context at %p for realm '%s'\n",
             __FUNCTION__, plgctx, realmname);
    *pplgctx = plgctx;
    retval = 0;

errout:
    if (retval)
        pkinit_server_plugin_fini_realm(context, plgctx);

    return retval;
}
