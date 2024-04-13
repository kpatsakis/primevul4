pkinit_process_td_trusted_certifiers(
    krb5_context context,
    pkinit_plg_crypto_context plg_cryptoctx,
    pkinit_req_crypto_context req_cryptoctx,
    pkinit_identity_crypto_context id_cryptoctx,
    krb5_external_principal_identifier **krb5_trusted_certifiers,
    int td_type)
{
    krb5_error_code retval = ENOMEM;
    STACK_OF(X509_NAME) *sk_xn = NULL;
    X509_NAME *xn = NULL;
    PKCS7_ISSUER_AND_SERIAL *is = NULL;
    ASN1_OCTET_STRING *id = NULL;
    const unsigned char *p = NULL;
    char buf[DN_BUF_LEN];
    int i = 0;

    if (td_type == TD_TRUSTED_CERTIFIERS)
        pkiDebug("received trusted certifiers\n");
    else
        pkiDebug("received invalid certificate\n");

    sk_xn = sk_X509_NAME_new_null();
    while(krb5_trusted_certifiers[i] != NULL) {
        if (krb5_trusted_certifiers[i]->subjectName.data != NULL) {
            p = (unsigned char *)krb5_trusted_certifiers[i]->subjectName.data;
            xn = d2i_X509_NAME(NULL, &p,
                               (int)krb5_trusted_certifiers[i]->subjectName.length);
            if (xn == NULL)
                goto cleanup;
            X509_NAME_oneline(xn, buf, sizeof(buf));
            if (td_type == TD_TRUSTED_CERTIFIERS)
                pkiDebug("#%d cert = %s is trusted by kdc\n", i, buf);
            else
                pkiDebug("#%d cert = %s is invalid\n", i, buf);
            sk_X509_NAME_push(sk_xn, xn);
        }

        if (krb5_trusted_certifiers[i]->issuerAndSerialNumber.data != NULL) {
            p = (unsigned char *)
                krb5_trusted_certifiers[i]->issuerAndSerialNumber.data;
            is = d2i_PKCS7_ISSUER_AND_SERIAL(NULL, &p,
                                             (int)krb5_trusted_certifiers[i]->issuerAndSerialNumber.length);
            if (is == NULL)
                goto cleanup;
            X509_NAME_oneline(is->issuer, buf, sizeof(buf));
            if (td_type == TD_TRUSTED_CERTIFIERS)
                pkiDebug("#%d issuer = %s serial = %ld is trusted bu kdc\n", i,
                         buf, ASN1_INTEGER_get(is->serial));
            else
                pkiDebug("#%d issuer = %s serial = %ld is invalid\n", i, buf,
                         ASN1_INTEGER_get(is->serial));
            PKCS7_ISSUER_AND_SERIAL_free(is);
        }

        if (krb5_trusted_certifiers[i]->subjectKeyIdentifier.data != NULL) {
            p = (unsigned char *)
                krb5_trusted_certifiers[i]->subjectKeyIdentifier.data;
            id = d2i_ASN1_OCTET_STRING(NULL, &p,
                                       (int)krb5_trusted_certifiers[i]->subjectKeyIdentifier.length);
            if (id == NULL)
                goto cleanup;
            /* XXX */
            ASN1_OCTET_STRING_free(id);
        }
        i++;
    }
    /* XXX Since we not doing anything with received trusted certifiers
     * return an error. this is the place where we can pick a different
     * client certificate based on the information in td_trusted_certifiers
     */
    retval = KRB5KDC_ERR_PREAUTH_FAILED;
cleanup:
    if (sk_xn != NULL)
        sk_X509_NAME_pop_free(sk_xn, X509_NAME_free);

    return retval;
}
