iakerb_acceptor_step(iakerb_ctx_id_t ctx,
                     int initialContextToken,
                     const gss_buffer_t input_token,
                     gss_buffer_t output_token)
{
    krb5_error_code code;
    krb5_data request = empty_data(), reply = empty_data();
    krb5_data realm = empty_data();
    OM_uint32 tmp;
    int tcp_only, use_master;
    krb5_ui_4 kdc_code;

    output_token->length = 0;
    output_token->value = NULL;

    if (ctx->count >= IAKERB_MAX_HOPS) {
        code = KRB5_KDC_UNREACH;
        goto cleanup;
    }

    code = iakerb_parse_token(ctx, initialContextToken, input_token, &realm,
                              NULL, &request);
    if (code != 0)
        goto cleanup;

    if (realm.length == 0 || request.length == 0) {
        code = KRB5_BAD_MSIZE;
        goto cleanup;
    }

    code = iakerb_save_token(ctx, input_token);
    if (code != 0)
        goto cleanup;

    for (tcp_only = 0; tcp_only <= 1; tcp_only++) {
        use_master = 0;
        code = krb5_sendto_kdc(ctx->k5c, &request, &realm,
                               &reply, &use_master, tcp_only);
        if (code == 0 && krb5_is_krb_error(&reply)) {
            krb5_error *error;

            code = decode_krb5_error(&reply, &error);
            if (code != 0)
                goto cleanup;
            kdc_code = error->error;
            krb5_free_error(ctx->k5c, error);
            if (kdc_code == KRB_ERR_RESPONSE_TOO_BIG) {
                krb5_free_data_contents(ctx->k5c, &reply);
                reply = empty_data();
                continue;
            }
        }
        break;
    }

    if (code == KRB5_KDC_UNREACH || code == KRB5_REALM_UNKNOWN) {
        krb5_error error;

        memset(&error, 0, sizeof(error));
        if (code == KRB5_KDC_UNREACH)
            error.error = KRB_AP_ERR_IAKERB_KDC_NO_RESPONSE;
        else if (code == KRB5_REALM_UNKNOWN)
            error.error = KRB_AP_ERR_IAKERB_KDC_NOT_FOUND;

        code = krb5_mk_error(ctx->k5c, &error, &reply);
        if (code != 0)
            goto cleanup;
    } else if (code != 0)
        goto cleanup;

    code = iakerb_make_token(ctx, &realm, NULL, &reply, 0, output_token);
    if (code != 0)
        goto cleanup;

    code = iakerb_save_token(ctx, output_token);
    if (code != 0)
        goto cleanup;

    ctx->count++;

cleanup:
    if (code != 0)
        gss_release_buffer(&tmp, output_token);
    /* request is a pointer into input_token, no need to free */
    krb5_free_data_contents(ctx->k5c, &realm);
    krb5_free_data_contents(ctx->k5c, &reply);

    return code;
}
