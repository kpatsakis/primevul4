server_check_dh(krb5_context context,
                pkinit_plg_crypto_context cryptoctx,
                pkinit_req_crypto_context req_cryptoctx,
                pkinit_identity_crypto_context id_cryptoctx,
                krb5_data *dh_params,
                int minbits)
{
    DH *dh = NULL;
    unsigned char *tmp = NULL;
    int dh_prime_bits;
    krb5_error_code retval = KRB5KDC_ERR_DH_KEY_PARAMETERS_NOT_ACCEPTED;

    tmp = (unsigned char *)dh_params->data;
    dh = DH_new();
    dh = pkinit_decode_dh_params(&dh, &tmp, dh_params->length);
    if (dh == NULL) {
        pkiDebug("failed to decode dhparams\n");
        goto cleanup;
    }

    /* KDC SHOULD check to see if the key parameters satisfy its policy */
    dh_prime_bits = BN_num_bits(dh->p);
    if (minbits && dh_prime_bits < minbits) {
        pkiDebug("client sent dh params with %d bits, we require %d\n",
                 dh_prime_bits, minbits);
        goto cleanup;
    }

    /* check dhparams is group 2 */
    if (pkinit_check_dh_params(cryptoctx->dh_1024->p,
                               dh->p, dh->g, dh->q) == 0) {
        retval = 0;
        goto cleanup;
    }

    /* check dhparams is group 14 */
    if (pkinit_check_dh_params(cryptoctx->dh_2048->p,
                               dh->p, dh->g, dh->q) == 0) {
        retval = 0;
        goto cleanup;
    }

    /* check dhparams is group 16 */
    if (pkinit_check_dh_params(cryptoctx->dh_4096->p,
                               dh->p, dh->g, dh->q) == 0) {
        retval = 0;
        goto cleanup;
    }

cleanup:
    if (retval == 0)
        req_cryptoctx->dh = dh;
    else
        DH_free(dh);

    return retval;
}
