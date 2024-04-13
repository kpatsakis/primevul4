pkinit_fini_plg_crypto(pkinit_plg_crypto_context cryptoctx)
{
    pkiDebug("%s: freeing context at %p\n", __FUNCTION__, cryptoctx);

    if (cryptoctx == NULL)
        return;
    pkinit_fini_pkinit_oids(cryptoctx);
    pkinit_fini_dh_params(cryptoctx);
    free(cryptoctx);
}
