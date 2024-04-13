pkinit_init_dh_params(pkinit_plg_crypto_context plgctx)
{
    krb5_error_code retval = ENOMEM;

    plgctx->dh_1024 = DH_new();
    if (plgctx->dh_1024 == NULL)
        goto cleanup;
    plgctx->dh_1024->p = BN_bin2bn(pkinit_1024_dhprime,
                                   sizeof(pkinit_1024_dhprime), NULL);
    if ((plgctx->dh_1024->g = BN_new()) == NULL ||
        (plgctx->dh_1024->q = BN_new()) == NULL)
        goto cleanup;
    BN_set_word(plgctx->dh_1024->g, DH_GENERATOR_2);
    BN_rshift1(plgctx->dh_1024->q, plgctx->dh_1024->p);

    plgctx->dh_2048 = DH_new();
    if (plgctx->dh_2048 == NULL)
        goto cleanup;
    plgctx->dh_2048->p = BN_bin2bn(pkinit_2048_dhprime,
                                   sizeof(pkinit_2048_dhprime), NULL);
    if ((plgctx->dh_2048->g = BN_new()) == NULL ||
        (plgctx->dh_2048->q = BN_new()) == NULL)
        goto cleanup;
    BN_set_word(plgctx->dh_2048->g, DH_GENERATOR_2);
    BN_rshift1(plgctx->dh_2048->q, plgctx->dh_2048->p);

    plgctx->dh_4096 = DH_new();
    if (plgctx->dh_4096 == NULL)
        goto cleanup;
    plgctx->dh_4096->p = BN_bin2bn(pkinit_4096_dhprime,
                                   sizeof(pkinit_4096_dhprime), NULL);
    if ((plgctx->dh_4096->g = BN_new()) == NULL ||
        (plgctx->dh_4096->q = BN_new()) == NULL)
        goto cleanup;
    BN_set_word(plgctx->dh_4096->g, DH_GENERATOR_2);
    BN_rshift1(plgctx->dh_4096->q, plgctx->dh_4096->p);

    retval = 0;

cleanup:
    if (retval)
        pkinit_fini_dh_params(plgctx);

    return retval;
}
