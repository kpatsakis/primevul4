pkinit_fini_dh_params(pkinit_plg_crypto_context plgctx)
{
    if (plgctx->dh_1024 != NULL)
        DH_free(plgctx->dh_1024);
    if (plgctx->dh_2048 != NULL)
        DH_free(plgctx->dh_2048);
    if (plgctx->dh_4096 != NULL)
        DH_free(plgctx->dh_4096);

    plgctx->dh_1024 = plgctx->dh_2048 = plgctx->dh_4096 = NULL;
}
