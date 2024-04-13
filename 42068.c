cms_signeddata_verify(krb5_context context,
                      pkinit_plg_crypto_context plgctx,
                      pkinit_req_crypto_context reqctx,
                      pkinit_identity_crypto_context idctx,
                      int cms_msg_type,
                      int require_crl_checking,
                      unsigned char *signed_data,
                      unsigned int signed_data_len,
                      unsigned char **data,
                      unsigned int *data_len,
                      unsigned char **authz_data,
                      unsigned int *authz_data_len,
                      int *is_signed)
{
    /*
     * Warning: Since most openssl functions do not set retval, large chunks of
     * this function assume that retval is always a failure and may go to
     * cleanup without setting retval explicitly. Make sure retval is not set
     * to 0 or errors such as signature verification failure may be converted
     * to success with significant security consequences.
     */
    krb5_error_code retval = KRB5KDC_ERR_PREAUTH_FAILED;
    CMS_ContentInfo *cms = NULL;
    BIO *out = NULL;
    int flags = CMS_NO_SIGNER_CERT_VERIFY;
    int valid_oid = 0;
    unsigned int i = 0;
    unsigned int vflags = 0, size = 0;
    const unsigned char *p = signed_data;
    STACK_OF(CMS_SignerInfo) *si_sk = NULL;
    CMS_SignerInfo *si = NULL;
    X509 *x = NULL;
    X509_STORE *store = NULL;
    X509_STORE_CTX cert_ctx;
    STACK_OF(X509) *signerCerts = NULL;
    STACK_OF(X509) *intermediateCAs = NULL;
    STACK_OF(X509_CRL) *signerRevoked = NULL;
    STACK_OF(X509_CRL) *revoked = NULL;
    STACK_OF(X509) *verified_chain = NULL;
    ASN1_OBJECT *oid = NULL;
    const ASN1_OBJECT *type = NULL, *etype = NULL;
    ASN1_OCTET_STRING **octets;
    krb5_external_principal_identifier **krb5_verified_chain = NULL;
    krb5_data *authz = NULL;
    char buf[DN_BUF_LEN];

#ifdef DEBUG_ASN1
    print_buffer_bin(signed_data, signed_data_len,
                     "/tmp/client_received_pkcs7_signeddata");
#endif
    if (is_signed)
        *is_signed = 1;
    /* Do this early enough to create the shadow OID for pkcs7-data if needed */
    oid = pkinit_pkcs7type2oid(plgctx, cms_msg_type);
    if (oid == NULL)
        goto cleanup;

    /* decode received CMS message */
    if ((cms = d2i_CMS_ContentInfo(NULL, &p, (int)signed_data_len)) == NULL) {
        unsigned long err = ERR_peek_error();
        krb5_set_error_message(context, retval, "%s\n",
                               ERR_error_string(err, NULL));
        pkiDebug("%s: failed to decode message: %s\n",
                 __FUNCTION__, ERR_error_string(err, NULL));
        goto cleanup;
    }
    etype = CMS_get0_eContentType(cms);

    /*
     * Prior to 1.10 the MIT client incorrectly omitted the pkinit structure
     * directly in a CMS ContentInfo rather than using SignedData with no
     * signers. Handle that case.
     */
    type = CMS_get0_type(cms);
    if (is_signed && !OBJ_cmp(type, oid)) {
        unsigned char *d;
        *is_signed = 0;
        octets = pkinit_CMS_get0_content_data(cms);
        if (!octets || ((*octets)->type != V_ASN1_OCTET_STRING)) {
            retval = KRB5KDC_ERR_PREAUTH_FAILED;
            krb5_set_error_message(context, retval,
                                   _("Invalid pkinit packet: octet string "
                                     "expected"));
            goto cleanup;
        }
        *data_len = ASN1_STRING_length(*octets);
        d = malloc(*data_len);
        if (d == NULL) {
            retval = ENOMEM;
            goto cleanup;
        }
        memcpy(d, ASN1_STRING_data(*octets),
               *data_len);
        *data = d;
        goto out;
    } else {
        /* Verify that the received message is CMS SignedData message. */
        if (OBJ_obj2nid(type) != NID_pkcs7_signed) {
            pkiDebug("Expected id-signedData CMS msg (received type = %d)\n",
                     OBJ_obj2nid(type));
            krb5_set_error_message(context, retval, _("wrong oid\n"));
            goto cleanup;
        }
    }

    /* setup to verify X509 certificate used to sign CMS message */
    if (!(store = X509_STORE_new()))
        goto cleanup;

    /* check if we are inforcing CRL checking */
    vflags = X509_V_FLAG_CRL_CHECK|X509_V_FLAG_CRL_CHECK_ALL;
    if (require_crl_checking)
        X509_STORE_set_verify_cb_func(store, openssl_callback);
    else
        X509_STORE_set_verify_cb_func(store, openssl_callback_ignore_crls);
    X509_STORE_set_flags(store, vflags);

    /*
     * Get the signer's information from the CMS message.  Match signer ID
     * against anchors and intermediate CAs in case no certs are present in the
     * SignedData.  If we start sending kdcPkId values in requests, we'll need
     * to match against the source of that information too.
     */
    CMS_set1_signers_certs(cms, NULL, 0);
    CMS_set1_signers_certs(cms, idctx->trustedCAs, CMS_NOINTERN);
    CMS_set1_signers_certs(cms, idctx->intermediateCAs, CMS_NOINTERN);
    if (((si_sk = CMS_get0_SignerInfos(cms)) == NULL) ||
        ((si = sk_CMS_SignerInfo_value(si_sk, 0)) == NULL)) {
        /* Not actually signed; anonymous case */
        if (!is_signed)
            goto cleanup;
        *is_signed = 0;
        /* We cannot use CMS_dataInit because there may be no digest */
        octets = pkinit_CMS_get0_content_signed(cms);
        if (octets)
            out = BIO_new_mem_buf((*octets)->data, (*octets)->length);
        if (out == NULL)
            goto cleanup;
    } else {
        pkinit_CMS_SignerInfo_get_cert(cms, si, &x);
        if (x == NULL)
            goto cleanup;

        /* create available CRL information (get local CRLs and include CRLs
         * received in the CMS message
         */
        signerRevoked = CMS_get1_crls(cms);
        if (idctx->revoked == NULL)
            revoked = signerRevoked;
        else if (signerRevoked == NULL)
            revoked = idctx->revoked;
        else {
            size = sk_X509_CRL_num(idctx->revoked);
            revoked = sk_X509_CRL_new_null();
            for (i = 0; i < size; i++)
                sk_X509_CRL_push(revoked, sk_X509_CRL_value(idctx->revoked, i));
            size = sk_X509_CRL_num(signerRevoked);
            for (i = 0; i < size; i++)
                sk_X509_CRL_push(revoked, sk_X509_CRL_value(signerRevoked, i));
        }

        /* create available intermediate CAs chains (get local intermediateCAs and
         * include the CA chain received in the CMS message
         */
        signerCerts = CMS_get1_certs(cms);
        if (idctx->intermediateCAs == NULL)
            intermediateCAs = signerCerts;
        else if (signerCerts == NULL)
            intermediateCAs = idctx->intermediateCAs;
        else {
            size = sk_X509_num(idctx->intermediateCAs);
            intermediateCAs = sk_X509_new_null();
            for (i = 0; i < size; i++) {
                sk_X509_push(intermediateCAs,
                             sk_X509_value(idctx->intermediateCAs, i));
            }
            size = sk_X509_num(signerCerts);
            for (i = 0; i < size; i++) {
                sk_X509_push(intermediateCAs, sk_X509_value(signerCerts, i));
            }
        }

        /* initialize x509 context with the received certificate and
         * trusted and intermediate CA chains and CRLs
         */
        if (!X509_STORE_CTX_init(&cert_ctx, store, x, intermediateCAs))
            goto cleanup;

        X509_STORE_CTX_set0_crls(&cert_ctx, revoked);

        /* add trusted CAs certificates for cert verification */
        if (idctx->trustedCAs != NULL)
            X509_STORE_CTX_trusted_stack(&cert_ctx, idctx->trustedCAs);
        else {
            pkiDebug("unable to find any trusted CAs\n");
            goto cleanup;
        }
#ifdef DEBUG_CERTCHAIN
        if (intermediateCAs != NULL) {
            size = sk_X509_num(intermediateCAs);
            pkiDebug("untrusted cert chain of size %d\n", size);
            for (i = 0; i < size; i++) {
                X509_NAME_oneline(X509_get_subject_name(
                                      sk_X509_value(intermediateCAs, i)), buf, sizeof(buf));
                pkiDebug("cert #%d: %s\n", i, buf);
            }
        }
        if (idctx->trustedCAs != NULL) {
            size = sk_X509_num(idctx->trustedCAs);
            pkiDebug("trusted cert chain of size %d\n", size);
            for (i = 0; i < size; i++) {
                X509_NAME_oneline(X509_get_subject_name(
                                      sk_X509_value(idctx->trustedCAs, i)), buf, sizeof(buf));
                pkiDebug("cert #%d: %s\n", i, buf);
            }
        }
        if (revoked != NULL) {
            size = sk_X509_CRL_num(revoked);
            pkiDebug("CRL chain of size %d\n", size);
            for (i = 0; i < size; i++) {
                X509_CRL *crl = sk_X509_CRL_value(revoked, i);
                X509_NAME_oneline(X509_CRL_get_issuer(crl), buf, sizeof(buf));
                pkiDebug("crls by CA #%d: %s\n", i , buf);
            }
        }
#endif

        i = X509_verify_cert(&cert_ctx);
        if (i <= 0) {
            int j = X509_STORE_CTX_get_error(&cert_ctx);

            reqctx->received_cert = X509_dup(cert_ctx.current_cert);
            switch(j) {
            case X509_V_ERR_CERT_REVOKED:
                retval = KRB5KDC_ERR_REVOKED_CERTIFICATE;
                break;
            case X509_V_ERR_UNABLE_TO_GET_CRL:
                retval = KRB5KDC_ERR_REVOCATION_STATUS_UNKNOWN;
                break;
            case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
            case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
                retval = KRB5KDC_ERR_CANT_VERIFY_CERTIFICATE;
                break;
            default:
                retval = KRB5KDC_ERR_INVALID_CERTIFICATE;
            }
            if (reqctx->received_cert == NULL)
                strlcpy(buf, "(none)", sizeof(buf));
            else
                X509_NAME_oneline(X509_get_subject_name(reqctx->received_cert),
                                  buf, sizeof(buf));
            pkiDebug("problem with cert DN = %s (error=%d) %s\n", buf, j,
                     X509_verify_cert_error_string(j));
            krb5_set_error_message(context, retval, "%s\n",
                                   X509_verify_cert_error_string(j));
#ifdef DEBUG_CERTCHAIN
            size = sk_X509_num(signerCerts);
            pkiDebug("received cert chain of size %d\n", size);
            for (j = 0; j < size; j++) {
                X509 *tmp_cert = sk_X509_value(signerCerts, j);
                X509_NAME_oneline(X509_get_subject_name(tmp_cert), buf, sizeof(buf));
                pkiDebug("cert #%d: %s\n", j, buf);
            }
#endif
        } else {
            /* retrieve verified certificate chain */
            if (cms_msg_type == CMS_SIGN_CLIENT || cms_msg_type == CMS_SIGN_DRAFT9)
                verified_chain = X509_STORE_CTX_get1_chain(&cert_ctx);
        }
        X509_STORE_CTX_cleanup(&cert_ctx);
        if (i <= 0)
            goto cleanup;
        out = BIO_new(BIO_s_mem());
        if (cms_msg_type == CMS_SIGN_DRAFT9)
            flags |= CMS_NOATTR;
        if (CMS_verify(cms, NULL, store, NULL, out, flags) == 0) {
            unsigned long err = ERR_peek_error();
            switch(ERR_GET_REASON(err)) {
            case PKCS7_R_DIGEST_FAILURE:
                retval = KRB5KDC_ERR_DIGEST_IN_SIGNED_DATA_NOT_ACCEPTED;
                break;
            case PKCS7_R_SIGNATURE_FAILURE:
            default:
                retval = KRB5KDC_ERR_INVALID_SIG;
            }
            pkiDebug("CMS Verification failure\n");
            krb5_set_error_message(context, retval, "%s\n",
                                   ERR_error_string(err, NULL));
            goto cleanup;
        }
    } /* message was signed */
    if (!OBJ_cmp(etype, oid))
        valid_oid = 1;
    else if (cms_msg_type == CMS_SIGN_DRAFT9) {
        /*
         * Various implementations of the pa-type 15 request use
         * different OIDS.  We check that the returned object
         * has any of the acceptable OIDs
         */
        ASN1_OBJECT *client_oid = NULL, *server_oid = NULL, *rsa_oid = NULL;
        client_oid = pkinit_pkcs7type2oid(plgctx, CMS_SIGN_CLIENT);
        server_oid = pkinit_pkcs7type2oid(plgctx, CMS_SIGN_SERVER);
        rsa_oid = pkinit_pkcs7type2oid(plgctx, CMS_ENVEL_SERVER);
        if (!OBJ_cmp(etype, client_oid) ||
            !OBJ_cmp(etype, server_oid) ||
            !OBJ_cmp(etype, rsa_oid))
            valid_oid = 1;
    }

    if (valid_oid)
        pkiDebug("CMS Verification successful\n");
    else {
        pkiDebug("wrong oid in eContentType\n");
        print_buffer(etype->data,
                     (unsigned int)etype->length);
        retval = KRB5KDC_ERR_PREAUTH_FAILED;
        krb5_set_error_message(context, retval, "wrong oid\n");
        goto cleanup;
    }

    /* transfer the data from CMS message into return buffer */
    for (size = 0;;) {
        int remain;
        retval = ENOMEM;
        if ((*data = realloc(*data, size + 1024 * 10)) == NULL)
            goto cleanup;
        remain = BIO_read(out, &((*data)[size]), 1024 * 10);
        if (remain <= 0)
            break;
        else
            size += remain;
    }
    *data_len = size;

    if (x) {
        reqctx->received_cert = X509_dup(x);

        /* generate authorization data */
        if (cms_msg_type == CMS_SIGN_CLIENT || cms_msg_type == CMS_SIGN_DRAFT9) {

            if (authz_data == NULL || authz_data_len == NULL)
                goto out;

            *authz_data = NULL;
            retval = create_identifiers_from_stack(verified_chain,
                                                   &krb5_verified_chain);
            if (retval) {
                pkiDebug("create_identifiers_from_stack failed\n");
                goto cleanup;
            }

            retval = k5int_encode_krb5_td_trusted_certifiers((const krb5_external_principal_identifier **)krb5_verified_chain, &authz);
            if (retval) {
                pkiDebug("encode_krb5_td_trusted_certifiers failed\n");
                goto cleanup;
            }
#ifdef DEBUG_ASN1
            print_buffer_bin((unsigned char *)authz->data, authz->length,
                             "/tmp/kdc_ad_initial_verified_cas");
#endif
            *authz_data = malloc(authz->length);
            if (*authz_data == NULL) {
                retval = ENOMEM;
                goto cleanup;
            }
            memcpy(*authz_data, authz->data, authz->length);
            *authz_data_len = authz->length;
        }
    }
out:
    retval = 0;

cleanup:
    if (out != NULL)
        BIO_free(out);
    if (store != NULL)
        X509_STORE_free(store);
    if (cms != NULL) {
        if (signerCerts != NULL)
            pkinit_CMS_free1_certs(signerCerts);
        if (idctx->intermediateCAs != NULL && signerCerts)
            sk_X509_free(intermediateCAs);
        if (signerRevoked != NULL)
            pkinit_CMS_free1_crls(signerRevoked);
        if (idctx->revoked != NULL && signerRevoked)
            sk_X509_CRL_free(revoked);
        CMS_ContentInfo_free(cms);
    }
    if (verified_chain != NULL)
        sk_X509_pop_free(verified_chain, X509_free);
    if (krb5_verified_chain != NULL)
        free_krb5_external_principal_identifier(&krb5_verified_chain);
    if (authz != NULL)
        krb5_free_data(context, authz);

    return retval;
}
