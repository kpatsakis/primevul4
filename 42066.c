cms_envelopeddata_create(krb5_context context,
                         pkinit_plg_crypto_context plgctx,
                         pkinit_req_crypto_context reqctx,
                         pkinit_identity_crypto_context idctx,
                         krb5_preauthtype pa_type,
                         int include_certchain,
                         unsigned char *key_pack,
                         unsigned int key_pack_len,
                         unsigned char **out,
                         unsigned int *out_len)
{

    krb5_error_code retval = ENOMEM;
    PKCS7 *p7 = NULL;
    BIO *in = NULL;
    unsigned char *p = NULL, *signed_data = NULL, *enc_data = NULL;
    int signed_data_len = 0, enc_data_len = 0, flags = PKCS7_BINARY;
    STACK_OF(X509) *encerts = NULL;
    const EVP_CIPHER *cipher = NULL;
    int cms_msg_type;

    /* create the PKCS7 SignedData portion of the PKCS7 EnvelopedData */
    switch ((int)pa_type) {
    case KRB5_PADATA_PK_AS_REQ_OLD:
    case KRB5_PADATA_PK_AS_REP_OLD:
        cms_msg_type = CMS_SIGN_DRAFT9;
        break;
    case KRB5_PADATA_PK_AS_REQ:
        cms_msg_type = CMS_ENVEL_SERVER;
        break;
    default:
        goto cleanup;
    }

    retval = cms_signeddata_create(context, plgctx, reqctx, idctx,
                                   cms_msg_type, include_certchain, key_pack, key_pack_len,
                                   &signed_data, (unsigned int *)&signed_data_len);
    if (retval) {
        pkiDebug("failed to create pkcs7 signed data\n");
        goto cleanup;
    }

    /* check we have client's certificate */
    if (reqctx->received_cert == NULL) {
        retval = KRB5KDC_ERR_PREAUTH_FAILED;
        goto cleanup;
    }
    encerts = sk_X509_new_null();
    sk_X509_push(encerts, reqctx->received_cert);

    cipher = EVP_des_ede3_cbc();
    in = BIO_new(BIO_s_mem());
    switch (pa_type) {
    case KRB5_PADATA_PK_AS_REQ:
        prepare_enc_data(signed_data, signed_data_len, &enc_data,
                         &enc_data_len);
        retval = BIO_write(in, enc_data, enc_data_len);
        if (retval != enc_data_len) {
            pkiDebug("BIO_write only wrote %d\n", retval);
            goto cleanup;
        }
        break;
    case KRB5_PADATA_PK_AS_REP_OLD:
    case KRB5_PADATA_PK_AS_REQ_OLD:
        retval = BIO_write(in, signed_data, signed_data_len);
        if (retval != signed_data_len) {
            pkiDebug("BIO_write only wrote %d\n", retval);
            goto cleanup;
        }
        break;
    default:
        retval = -1;
        goto cleanup;
    }

    p7 = PKCS7_encrypt(encerts, in, cipher, flags);
    if (p7 == NULL) {
        pkiDebug("failed to encrypt PKCS7 object\n");
        retval = -1;
        goto cleanup;
    }
    switch (pa_type) {
    case KRB5_PADATA_PK_AS_REQ:
        p7->d.enveloped->enc_data->content_type =
            OBJ_nid2obj(NID_pkcs7_signed);
        break;
    case KRB5_PADATA_PK_AS_REP_OLD:
    case KRB5_PADATA_PK_AS_REQ_OLD:
        p7->d.enveloped->enc_data->content_type =
            OBJ_nid2obj(NID_pkcs7_data);
        break;
        break;
        break;
        break;
    }

    *out_len = i2d_PKCS7(p7, NULL);
    if (!*out_len || (p = *out = malloc(*out_len)) == NULL) {
        retval = ENOMEM;
        goto cleanup;
    }
    retval = i2d_PKCS7(p7, &p);
    if (!retval) {
        pkiDebug("unable to write pkcs7 object\n");
        goto cleanup;
    }
    retval = 0;

#ifdef DEBUG_ASN1
    print_buffer_bin(*out, *out_len, "/tmp/kdc_enveloped_data");
#endif

cleanup:
    if (p7 != NULL)
        PKCS7_free(p7);
    if (in != NULL)
        BIO_free(in);
    free(signed_data);
    free(enc_data);
    if (encerts != NULL)
        sk_X509_free(encerts);

    return retval;
}
