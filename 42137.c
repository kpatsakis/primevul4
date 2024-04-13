pkinit_process_td_dh_params(krb5_context context,
                            pkinit_plg_crypto_context cryptoctx,
                            pkinit_req_crypto_context req_cryptoctx,
                            pkinit_identity_crypto_context id_cryptoctx,
                            krb5_algorithm_identifier **algId,
                            int *new_dh_size)
{
    krb5_error_code retval = KRB5KDC_ERR_DH_KEY_PARAMETERS_NOT_ACCEPTED;
    int i = 0, use_sent_dh = 0, ok = 0;

    pkiDebug("dh parameters\n");

    while (algId[i] != NULL) {
        DH *dh = NULL;
        unsigned char *tmp = NULL;
        int dh_prime_bits = 0;

        if (algId[i]->algorithm.length != dh_oid.length ||
            memcmp(algId[i]->algorithm.data, dh_oid.data, dh_oid.length))
            goto cleanup;

        tmp = (unsigned char *)algId[i]->parameters.data;
        dh = DH_new();
        dh = pkinit_decode_dh_params(&dh, &tmp, algId[i]->parameters.length);
        dh_prime_bits = BN_num_bits(dh->p);
        pkiDebug("client sent %d DH bits server prefers %d DH bits\n",
                 *new_dh_size, dh_prime_bits);
        switch(dh_prime_bits) {
        case 1024:
            if (pkinit_check_dh_params(cryptoctx->dh_1024->p, dh->p,
                                       dh->g, dh->q) == 0) {
                *new_dh_size = 1024;
                ok = 1;
            }
            break;
        case 2048:
            if (pkinit_check_dh_params(cryptoctx->dh_2048->p, dh->p,
                                       dh->g, dh->q) == 0) {
                *new_dh_size = 2048;
                ok = 1;
            }
            break;
        case 4096:
            if (pkinit_check_dh_params(cryptoctx->dh_4096->p, dh->p,
                                       dh->g, dh->q) == 0) {
                *new_dh_size = 4096;
                ok = 1;
            }
            break;
        default:
            break;
        }
        if (!ok) {
            DH_check(dh, &retval);
            if (retval != 0) {
                pkiDebug("DH parameters provided by server are unacceptable\n");
                retval = KRB5KDC_ERR_DH_KEY_PARAMETERS_NOT_ACCEPTED;
            }
            else {
                use_sent_dh = 1;
                ok = 1;
            }
        }
        if (!use_sent_dh)
            DH_free(dh);
        if (ok) {
            if (req_cryptoctx->dh != NULL) {
                DH_free(req_cryptoctx->dh);
                req_cryptoctx->dh = NULL;
            }
            if (use_sent_dh)
                req_cryptoctx->dh = dh;
            break;
        }
        i++;
    }

    if (ok)
        retval = 0;

cleanup:
    return retval;
}
