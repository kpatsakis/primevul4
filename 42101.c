pkcs7_dataDecode(krb5_context context,
                 pkinit_identity_crypto_context id_cryptoctx,
                 PKCS7 *p7)
{
    int i = 0;
    unsigned int jj = 0, tmp_len = 0;
    BIO *out=NULL,*etmp=NULL,*bio=NULL;
    unsigned char *tmp=NULL;
    ASN1_OCTET_STRING *data_body=NULL;
    const EVP_CIPHER *evp_cipher=NULL;
    EVP_CIPHER_CTX *evp_ctx=NULL;
    X509_ALGOR *enc_alg=NULL;
    STACK_OF(PKCS7_RECIP_INFO) *rsk=NULL;
    PKCS7_RECIP_INFO *ri=NULL;
    X509 *cert = sk_X509_value(id_cryptoctx->my_certs,
                               id_cryptoctx->cert_index);

    p7->state=PKCS7_S_HEADER;

    rsk=p7->d.enveloped->recipientinfo;
    enc_alg=p7->d.enveloped->enc_data->algorithm;
    data_body=p7->d.enveloped->enc_data->enc_data;
    evp_cipher=EVP_get_cipherbyobj(enc_alg->algorithm);
    if (evp_cipher == NULL) {
        PKCS7err(PKCS7_F_PKCS7_DATADECODE,PKCS7_R_UNSUPPORTED_CIPHER_TYPE);
        goto cleanup;
    }

    if ((etmp=BIO_new(BIO_f_cipher())) == NULL) {
        PKCS7err(PKCS7_F_PKCS7_DATADECODE,ERR_R_BIO_LIB);
        goto cleanup;
    }

    /* It was encrypted, we need to decrypt the secret key
     * with the private key */

    /* Find the recipientInfo which matches the passed certificate
     * (if any)
     */

    if (cert) {
        for (i=0; i<sk_PKCS7_RECIP_INFO_num(rsk); i++) {
            int tmp_ret = 0;
            ri=sk_PKCS7_RECIP_INFO_value(rsk,i);
            tmp_ret = X509_NAME_cmp(ri->issuer_and_serial->issuer,
                                    cert->cert_info->issuer);
            if (!tmp_ret) {
                tmp_ret = M_ASN1_INTEGER_cmp(cert->cert_info->serialNumber,
                                             ri->issuer_and_serial->serial);
                if (!tmp_ret)
                    break;
            }
            ri=NULL;
        }
        if (ri == NULL) {
            PKCS7err(PKCS7_F_PKCS7_DATADECODE,
                     PKCS7_R_NO_RECIPIENT_MATCHES_CERTIFICATE);
            goto cleanup;
        }

    }

    /* If we haven't got a certificate try each ri in turn */

    if (cert == NULL) {
        for (i=0; i<sk_PKCS7_RECIP_INFO_num(rsk); i++) {
            ri=sk_PKCS7_RECIP_INFO_value(rsk,i);
            jj = pkinit_decode_data(context, id_cryptoctx,
                                    M_ASN1_STRING_data(ri->enc_key),
                                    (unsigned int) M_ASN1_STRING_length(ri->enc_key),
                                    &tmp, &tmp_len);
            if (jj) {
                PKCS7err(PKCS7_F_PKCS7_DATADECODE, ERR_R_EVP_LIB);
                goto cleanup;
            }

            if (!jj && tmp_len > 0) {
                jj = tmp_len;
                break;
            }

            ERR_clear_error();
            ri = NULL;
        }

        if (ri == NULL) {
            PKCS7err(PKCS7_F_PKCS7_DATADECODE, PKCS7_R_NO_RECIPIENT_MATCHES_KEY);
            goto cleanup;
        }
    }
    else {
        jj = pkinit_decode_data(context, id_cryptoctx,
                                M_ASN1_STRING_data(ri->enc_key),
                                (unsigned int) M_ASN1_STRING_length(ri->enc_key),
                                &tmp, &tmp_len);
        if (jj || tmp_len <= 0) {
            PKCS7err(PKCS7_F_PKCS7_DATADECODE, ERR_R_EVP_LIB);
            goto cleanup;
        }
        jj = tmp_len;
    }

    evp_ctx=NULL;
    BIO_get_cipher_ctx(etmp,&evp_ctx);
    if (EVP_CipherInit_ex(evp_ctx,evp_cipher,NULL,NULL,NULL,0) <= 0)
        goto cleanup;
    if (EVP_CIPHER_asn1_to_param(evp_ctx,enc_alg->parameter) < 0)
        goto cleanup;

    if (jj != (unsigned) EVP_CIPHER_CTX_key_length(evp_ctx)) {
        /* Some S/MIME clients don't use the same key
         * and effective key length. The key length is
         * determined by the size of the decrypted RSA key.
         */
        if(!EVP_CIPHER_CTX_set_key_length(evp_ctx, (int)jj)) {
            PKCS7err(PKCS7_F_PKCS7_DATADECODE,
                     PKCS7_R_DECRYPTED_KEY_IS_WRONG_LENGTH);
            goto cleanup;
        }
    }
    if (EVP_CipherInit_ex(evp_ctx,NULL,NULL,tmp,NULL,0) <= 0)
        goto cleanup;

    OPENSSL_cleanse(tmp,jj);

    if (out == NULL)
        out=etmp;
    else
        BIO_push(out,etmp);
    etmp=NULL;

    if (data_body->length > 0)
        bio = BIO_new_mem_buf(data_body->data, data_body->length);
    else {
        bio=BIO_new(BIO_s_mem());
        BIO_set_mem_eof_return(bio,0);
    }
    BIO_push(out,bio);
    bio=NULL;

    if (0) {
    cleanup:
        if (out != NULL) BIO_free_all(out);
        if (etmp != NULL) BIO_free_all(etmp);
        if (bio != NULL) BIO_free_all(bio);
        out=NULL;
    }

    if (tmp != NULL)
        free(tmp);

    return(out);
}
