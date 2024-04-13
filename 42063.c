client_create_dh(krb5_context context,
                 pkinit_plg_crypto_context plg_cryptoctx,
                 pkinit_req_crypto_context cryptoctx,
                 pkinit_identity_crypto_context id_cryptoctx,
                 int dh_size,
                 unsigned char **dh_params,
                 unsigned int *dh_params_len,
                 unsigned char **dh_pubkey,
                 unsigned int *dh_pubkey_len)
{
    krb5_error_code retval = KRB5KDC_ERR_PREAUTH_FAILED;
    unsigned char *buf = NULL;
    int dh_err = 0;
    ASN1_INTEGER *pub_key = NULL;

    if (cryptoctx->dh == NULL) {
        if ((cryptoctx->dh = DH_new()) == NULL)
            goto cleanup;
        if ((cryptoctx->dh->g = BN_new()) == NULL ||
            (cryptoctx->dh->q = BN_new()) == NULL)
            goto cleanup;

        switch(dh_size) {
        case 1024:
            pkiDebug("client uses 1024 DH keys\n");
            cryptoctx->dh->p = get_rfc2409_prime_1024(NULL);
            break;
        case 2048:
            pkiDebug("client uses 2048 DH keys\n");
            cryptoctx->dh->p = BN_bin2bn(pkinit_2048_dhprime,
                                         sizeof(pkinit_2048_dhprime), NULL);
            break;
        case 4096:
            pkiDebug("client uses 4096 DH keys\n");
            cryptoctx->dh->p = BN_bin2bn(pkinit_4096_dhprime,
                                         sizeof(pkinit_4096_dhprime), NULL);
            break;
        default:
            goto cleanup;
        }

        BN_set_word((cryptoctx->dh->g), DH_GENERATOR_2);
        BN_rshift1(cryptoctx->dh->q, cryptoctx->dh->p);
    }

    DH_generate_key(cryptoctx->dh);
    DH_check(cryptoctx->dh, &dh_err);
    if (dh_err != 0) {
        pkiDebug("Warning: dh_check failed with %d\n", dh_err);
        if (dh_err & DH_CHECK_P_NOT_PRIME)
            pkiDebug("p value is not prime\n");
        if (dh_err & DH_CHECK_P_NOT_SAFE_PRIME)
            pkiDebug("p value is not a safe prime\n");
        if (dh_err & DH_UNABLE_TO_CHECK_GENERATOR)
            pkiDebug("unable to check the generator value\n");
        if (dh_err & DH_NOT_SUITABLE_GENERATOR)
            pkiDebug("the g value is not a generator\n");
    }
#ifdef DEBUG_DH
    print_dh(cryptoctx->dh, "client's DH params\n");
    print_pubkey(cryptoctx->dh->pub_key, "client's pub_key=");
#endif

    DH_check_pub_key(cryptoctx->dh, cryptoctx->dh->pub_key, &dh_err);
    if (dh_err != 0) {
        pkiDebug("dh_check_pub_key failed with %d\n", dh_err);
        goto cleanup;
    }

    /* pack DHparams */
    /* aglo: usually we could just call i2d_DHparams to encode DH params
     * however, PKINIT requires RFC3279 encoding and openssl does pkcs#3.
     */
    retval = pkinit_encode_dh_params(cryptoctx->dh->p, cryptoctx->dh->g,
                                     cryptoctx->dh->q, dh_params, dh_params_len);
    if (retval)
        goto cleanup;

    /* pack DH public key */
    /* Diffie-Hellman public key must be ASN1 encoded as an INTEGER; this
     * encoding shall be used as the contents (the value) of the
     * subjectPublicKey component (a BIT STRING) of the SubjectPublicKeyInfo
     * data element
     */
    if ((pub_key = BN_to_ASN1_INTEGER(cryptoctx->dh->pub_key, NULL)) == NULL)
        goto cleanup;
    *dh_pubkey_len = i2d_ASN1_INTEGER(pub_key, NULL);
    if ((buf = *dh_pubkey = malloc(*dh_pubkey_len)) == NULL) {
        retval  = ENOMEM;
        goto cleanup;
    }
    i2d_ASN1_INTEGER(pub_key, &buf);

    if (pub_key != NULL)
        ASN1_INTEGER_free(pub_key);

    retval = 0;
    return retval;

cleanup:
    if (cryptoctx->dh != NULL)
        DH_free(cryptoctx->dh);
    cryptoctx->dh = NULL;
    free(*dh_params);
    *dh_params = NULL;
    free(*dh_pubkey);
    *dh_pubkey = NULL;
    if (pub_key != NULL)
        ASN1_INTEGER_free(pub_key);

    return retval;
}
