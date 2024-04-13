cms_envelopeddata_verify(krb5_context context,
                         pkinit_plg_crypto_context plg_cryptoctx,
                         pkinit_req_crypto_context req_cryptoctx,
                         pkinit_identity_crypto_context id_cryptoctx,
                         krb5_preauthtype pa_type,
                         int require_crl_checking,
                         unsigned char *enveloped_data,
                         unsigned int enveloped_data_len,
                         unsigned char **data,
                         unsigned int *data_len)
{
    krb5_error_code retval = KRB5KDC_ERR_PREAUTH_FAILED;
    PKCS7 *p7 = NULL;
    BIO *out = NULL;
    int i = 0;
    unsigned int size = 0;
    const unsigned char *p = enveloped_data;
    unsigned int tmp_buf_len = 0, tmp_buf2_len = 0, vfy_buf_len = 0;
    unsigned char *tmp_buf = NULL, *tmp_buf2 = NULL, *vfy_buf = NULL;
    int msg_type = 0;

#ifdef DEBUG_ASN1
    print_buffer_bin(enveloped_data, enveloped_data_len,
                     "/tmp/client_envelopeddata");
#endif
    /* decode received PKCS7 message */
    if ((p7 = d2i_PKCS7(NULL, &p, (int)enveloped_data_len)) == NULL) {
        unsigned long err = ERR_peek_error();
        pkiDebug("failed to decode pkcs7\n");
        krb5_set_error_message(context, retval, "%s\n",
                               ERR_error_string(err, NULL));
        goto cleanup;
    }

    /* verify that the received message is PKCS7 EnvelopedData message */
    if (OBJ_obj2nid(p7->type) != NID_pkcs7_enveloped) {
        pkiDebug("Expected id-enveloped PKCS7 msg (received type = %d)\n",
                 OBJ_obj2nid(p7->type));
        krb5_set_error_message(context, retval, "wrong oid\n");
        goto cleanup;
    }

    /* decrypt received PKCS7 message */
    out = BIO_new(BIO_s_mem());
    if (pkcs7_decrypt(context, id_cryptoctx, p7, out)) {
        pkiDebug("PKCS7 decryption successful\n");
    } else {
        unsigned long err = ERR_peek_error();
        if (err != 0)
            krb5_set_error_message(context, retval, "%s\n",
                                   ERR_error_string(err, NULL));
        pkiDebug("PKCS7 decryption failed\n");
        goto cleanup;
    }

    /* transfer the decoded PKCS7 SignedData message into a separate buffer */
    for (;;) {
        if ((tmp_buf = realloc(tmp_buf, size + 1024 * 10)) == NULL)
            goto cleanup;
        i = BIO_read(out, &(tmp_buf[size]), 1024 * 10);
        if (i <= 0)
            break;
        else
            size += i;
    }
    tmp_buf_len = size;

#ifdef DEBUG_ASN1
    print_buffer_bin(tmp_buf, tmp_buf_len, "/tmp/client_enc_keypack");
#endif
    /* verify PKCS7 SignedData message */
    switch (pa_type) {
    case KRB5_PADATA_PK_AS_REP:
        msg_type = CMS_ENVEL_SERVER;

        break;
    case KRB5_PADATA_PK_AS_REP_OLD:
        msg_type = CMS_SIGN_DRAFT9;
        break;
    default:
        pkiDebug("%s: unrecognized pa_type = %d\n", __FUNCTION__, pa_type);
        retval = KRB5KDC_ERR_PREAUTH_FAILED;
        goto cleanup;
    }
    /*
     * If this is the RFC style, wrap the signed data to make
     * decoding easier in the verify routine.
     * For draft9-compatible, we don't do anything because it
     * is already wrapped.
     */
#ifdef LONGHORN_BETA_COMPAT
    /*
     * The Longhorn server returns the expected RFC-style data, but
     * it is missing the sequence tag and length, so it requires
     * special processing when wrapping.
     * This will hopefully be fixed before the final release and
     * this can all be removed.
     */
    if (msg_type == CMS_ENVEL_SERVER || longhorn == 1) {
        retval = wrap_signeddata(tmp_buf, tmp_buf_len,
                                 &tmp_buf2, &tmp_buf2_len, longhorn);
        if (retval) {
            pkiDebug("failed to encode signeddata\n");
            goto cleanup;
        }
        vfy_buf = tmp_buf2;
        vfy_buf_len = tmp_buf2_len;

    } else {
        vfy_buf = tmp_buf;
        vfy_buf_len = tmp_buf_len;
    }
#else
    if (msg_type == CMS_ENVEL_SERVER) {
        retval = wrap_signeddata(tmp_buf, tmp_buf_len,
                                 &tmp_buf2, &tmp_buf2_len);
        if (retval) {
            pkiDebug("failed to encode signeddata\n");
            goto cleanup;
        }
        vfy_buf = tmp_buf2;
        vfy_buf_len = tmp_buf2_len;

    } else {
        vfy_buf = tmp_buf;
        vfy_buf_len = tmp_buf_len;
    }
#endif

#ifdef DEBUG_ASN1
    print_buffer_bin(vfy_buf, vfy_buf_len, "/tmp/client_enc_keypack2");
#endif

    retval = cms_signeddata_verify(context, plg_cryptoctx, req_cryptoctx,
                                   id_cryptoctx, msg_type,
                                   require_crl_checking,
                                   vfy_buf, vfy_buf_len,
                                   data, data_len, NULL, NULL, NULL);

    if (!retval)
        pkiDebug("PKCS7 Verification Success\n");
    else {
        pkiDebug("PKCS7 Verification Failure\n");
        goto cleanup;
    }

    retval = 0;

cleanup:

    if (p7 != NULL)
        PKCS7_free(p7);
    if (out != NULL)
        BIO_free(out);
    free(tmp_buf);
    free(tmp_buf2);

    return retval;
}
