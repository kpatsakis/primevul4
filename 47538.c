iakerb_make_token(iakerb_ctx_id_t ctx,
                  krb5_data *realm,
                  krb5_data *cookie,
                  krb5_data *request,
                  int initialContextToken,
                  gss_buffer_t token)
{
    krb5_error_code code;
    krb5_iakerb_header iah;
    krb5_data *data = NULL;
    char *p;
    unsigned int tokenSize;
    unsigned char *q;

    token->value = NULL;
    token->length = 0;

    /*
     * Assemble the IAKERB-HEADER from the realm and cookie
     */
    memset(&iah, 0, sizeof(iah));
    iah.target_realm = *realm;
    iah.cookie = cookie;

    code = encode_krb5_iakerb_header(&iah, &data);
    if (code != 0)
        goto cleanup;

    /*
     * Concatenate Kerberos request.
     */
    p = realloc(data->data, data->length + request->length);
    if (p == NULL) {
        code = ENOMEM;
        goto cleanup;
    }
    data->data = p;

    if (request->length > 0)
        memcpy(data->data + data->length, request->data, request->length);
    data->length += request->length;

    if (initialContextToken)
        tokenSize = g_token_size(gss_mech_iakerb, data->length);
    else
        tokenSize = 2 + data->length;

    token->value = q = gssalloc_malloc(tokenSize);
    if (q == NULL) {
        code = ENOMEM;
        goto cleanup;
    }
    token->length = tokenSize;

    if (initialContextToken) {
        g_make_token_header(gss_mech_iakerb, data->length, &q,
                            IAKERB_TOK_PROXY);
    } else {
        store_16_be(IAKERB_TOK_PROXY, q);
        q += 2;
    }
    memcpy(q, data->data, data->length);
    q += data->length;

    assert(q == (unsigned char *)token->value + token->length);

cleanup:
    krb5_free_data(ctx->k5c, data);

    return code;
}
