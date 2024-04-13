iakerb_parse_token(iakerb_ctx_id_t ctx,
                   int initialContextToken,
                   const gss_buffer_t token,
                   krb5_data *realm,
                   krb5_data **cookie,
                   krb5_data *request)
{
    krb5_error_code code;
    krb5_iakerb_header *iah = NULL;
    unsigned int bodysize, lenlen;
    int length;
    unsigned char *ptr;
    int flags = 0;
    krb5_data data;

    if (token == GSS_C_NO_BUFFER || token->length == 0) {
        code = KRB5_BAD_MSIZE;
        goto cleanup;
    }

    if (initialContextToken)
        flags |= G_VFY_TOKEN_HDR_WRAPPER_REQUIRED;

    ptr = token->value;

    code = g_verify_token_header(gss_mech_iakerb,
                                 &bodysize, &ptr,
                                 IAKERB_TOK_PROXY,
                                 token->length, flags);
    if (code != 0)
        goto cleanup;

    data.data = (char *)ptr;

    if (bodysize-- == 0 || *ptr++ != 0x30 /* SEQUENCE */) {
        code = ASN1_BAD_ID;
        goto cleanup;
    }

    length = gssint_get_der_length(&ptr, bodysize, &lenlen);
    if (length < 0 || bodysize - lenlen < (unsigned int)length) {
        code = KRB5_BAD_MSIZE;
        goto cleanup;
    }
    data.length = 1 /* SEQUENCE */ + lenlen + length;

    ptr += length;
    bodysize -= (lenlen + length);

    code = decode_krb5_iakerb_header(&data, &iah);
    if (code != 0)
        goto cleanup;

    if (realm != NULL) {
        *realm = iah->target_realm;
        iah->target_realm.data = NULL;
    }

    if (cookie != NULL) {
        *cookie = iah->cookie;
        iah->cookie = NULL;
    }

    request->data = (char *)ptr;
    request->length = bodysize;

    assert(request->data + request->length ==
           (char *)token->value + token->length);

cleanup:
    krb5_free_iakerb_header(ctx->k5c, iah);

    return code;
}
