pkinit_create_td_dh_parameters(krb5_context context,
                               pkinit_plg_crypto_context plg_cryptoctx,
                               pkinit_req_crypto_context req_cryptoctx,
                               pkinit_identity_crypto_context id_cryptoctx,
                               pkinit_plg_opts *opts,
                               krb5_pa_data ***e_data_out)
{
    krb5_error_code retval = ENOMEM;
    unsigned int buf1_len = 0, buf2_len = 0, buf3_len = 0, i = 0;
    unsigned char *buf1 = NULL, *buf2 = NULL, *buf3 = NULL;
    krb5_pa_data **pa_data = NULL;
    krb5_data *encoded_algId = NULL;
    krb5_algorithm_identifier **algId = NULL;

    if (opts->dh_min_bits > 4096)
        goto cleanup;

    if (opts->dh_min_bits <= 1024) {
        retval = pkinit_encode_dh_params(plg_cryptoctx->dh_1024->p,
                                         plg_cryptoctx->dh_1024->g, plg_cryptoctx->dh_1024->q,
                                         &buf1, &buf1_len);
        if (retval)
            goto cleanup;
    }
    if (opts->dh_min_bits <= 2048) {
        retval = pkinit_encode_dh_params(plg_cryptoctx->dh_2048->p,
                                         plg_cryptoctx->dh_2048->g, plg_cryptoctx->dh_2048->q,
                                         &buf2, &buf2_len);
        if (retval)
            goto cleanup;
    }
    retval = pkinit_encode_dh_params(plg_cryptoctx->dh_4096->p,
                                     plg_cryptoctx->dh_4096->g, plg_cryptoctx->dh_4096->q,
                                     &buf3, &buf3_len);
    if (retval)
        goto cleanup;

    if (opts->dh_min_bits <= 1024) {
        algId = malloc(4 * sizeof(krb5_algorithm_identifier *));
        if (algId == NULL)
            goto cleanup;
        algId[3] = NULL;
        algId[0] = malloc(sizeof(krb5_algorithm_identifier));
        if (algId[0] == NULL)
            goto cleanup;
        algId[0]->parameters.data = malloc(buf2_len);
        if (algId[0]->parameters.data == NULL)
            goto cleanup;
        memcpy(algId[0]->parameters.data, buf2, buf2_len);
        algId[0]->parameters.length = buf2_len;
        algId[0]->algorithm = dh_oid;

        algId[1] = malloc(sizeof(krb5_algorithm_identifier));
        if (algId[1] == NULL)
            goto cleanup;
        algId[1]->parameters.data = malloc(buf3_len);
        if (algId[1]->parameters.data == NULL)
            goto cleanup;
        memcpy(algId[1]->parameters.data, buf3, buf3_len);
        algId[1]->parameters.length = buf3_len;
        algId[1]->algorithm = dh_oid;

        algId[2] = malloc(sizeof(krb5_algorithm_identifier));
        if (algId[2] == NULL)
            goto cleanup;
        algId[2]->parameters.data = malloc(buf1_len);
        if (algId[2]->parameters.data == NULL)
            goto cleanup;
        memcpy(algId[2]->parameters.data, buf1, buf1_len);
        algId[2]->parameters.length = buf1_len;
        algId[2]->algorithm = dh_oid;

    } else if (opts->dh_min_bits <= 2048) {
        algId = malloc(3 * sizeof(krb5_algorithm_identifier *));
        if (algId == NULL)
            goto cleanup;
        algId[2] = NULL;
        algId[0] = malloc(sizeof(krb5_algorithm_identifier));
        if (algId[0] == NULL)
            goto cleanup;
        algId[0]->parameters.data = malloc(buf2_len);
        if (algId[0]->parameters.data == NULL)
            goto cleanup;
        memcpy(algId[0]->parameters.data, buf2, buf2_len);
        algId[0]->parameters.length = buf2_len;
        algId[0]->algorithm = dh_oid;

        algId[1] = malloc(sizeof(krb5_algorithm_identifier));
        if (algId[1] == NULL)
            goto cleanup;
        algId[1]->parameters.data = malloc(buf3_len);
        if (algId[1]->parameters.data == NULL)
            goto cleanup;
        memcpy(algId[1]->parameters.data, buf3, buf3_len);
        algId[1]->parameters.length = buf3_len;
        algId[1]->algorithm = dh_oid;

    } else if (opts->dh_min_bits <= 4096) {
        algId = malloc(2 * sizeof(krb5_algorithm_identifier *));
        if (algId == NULL)
            goto cleanup;
        algId[1] = NULL;
        algId[0] = malloc(sizeof(krb5_algorithm_identifier));
        if (algId[0] == NULL)
            goto cleanup;
        algId[0]->parameters.data = malloc(buf3_len);
        if (algId[0]->parameters.data == NULL)
            goto cleanup;
        memcpy(algId[0]->parameters.data, buf3, buf3_len);
        algId[0]->parameters.length = buf3_len;
        algId[0]->algorithm = dh_oid;

    }
    retval = k5int_encode_krb5_td_dh_parameters((const krb5_algorithm_identifier **)algId, &encoded_algId);
    if (retval)
        goto cleanup;
#ifdef DEBUG_ASN1
    print_buffer_bin((unsigned char *)encoded_algId->data,
                     encoded_algId->length, "/tmp/kdc_td_dh_params");
#endif
    pa_data = malloc(2 * sizeof(krb5_pa_data *));
    if (pa_data == NULL) {
        retval = ENOMEM;
        goto cleanup;
    }
    pa_data[1] = NULL;
    pa_data[0] = malloc(sizeof(krb5_pa_data));
    if (pa_data[0] == NULL) {
        free(pa_data);
        retval = ENOMEM;
        goto cleanup;
    }
    pa_data[0]->pa_type = TD_DH_PARAMETERS;
    pa_data[0]->length = encoded_algId->length;
    pa_data[0]->contents = (krb5_octet *)encoded_algId->data;
    *e_data_out = pa_data;
    retval = 0;
cleanup:

    free(buf1);
    free(buf2);
    free(buf3);
    free(encoded_algId);

    if (algId != NULL) {
        while(algId[i] != NULL) {
            free(algId[i]->parameters.data);
            free(algId[i]);
            i++;
        }
        free(algId);
    }

    return retval;
}
