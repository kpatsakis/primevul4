pkinit_server_plugin_fini_realm(krb5_context context, pkinit_kdc_context plgctx)
{
    if (plgctx == NULL)
        return;

    pkinit_fini_kdc_profile(context, plgctx);
    pkinit_fini_identity_opts(plgctx->idopts);
    pkinit_fini_identity_crypto(plgctx->idctx);
    pkinit_fini_plg_crypto(plgctx->cryptoctx);
    pkinit_fini_plg_opts(plgctx->opts);
    free(plgctx->realmname);
    free(plgctx);
}
