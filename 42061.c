create_identifiers_from_stack(STACK_OF(X509) *sk,
                              krb5_external_principal_identifier *** ids)
{
    krb5_error_code retval = ENOMEM;
    int i = 0, sk_size = sk_X509_num(sk);
    krb5_external_principal_identifier **krb5_cas = NULL;
    X509 *x = NULL;
    X509_NAME *xn = NULL;
    unsigned char *p = NULL;
    int len = 0;
    PKCS7_ISSUER_AND_SERIAL *is = NULL;
    char buf[DN_BUF_LEN];

    *ids = NULL;

    krb5_cas =
        malloc((sk_size + 1) * sizeof(krb5_external_principal_identifier *));
    if (krb5_cas == NULL)
        return ENOMEM;
    krb5_cas[sk_size] = NULL;

    for (i = 0; i < sk_size; i++) {
        krb5_cas[i] = malloc(sizeof(krb5_external_principal_identifier));

        x = sk_X509_value(sk, i);

        X509_NAME_oneline(X509_get_subject_name(x), buf, sizeof(buf));
        pkiDebug("#%d cert= %s\n", i, buf);

        /* fill-in subjectName */
        krb5_cas[i]->subjectName.magic = 0;
        krb5_cas[i]->subjectName.length = 0;
        krb5_cas[i]->subjectName.data = NULL;

        xn = X509_get_subject_name(x);
        len = i2d_X509_NAME(xn, NULL);
        if ((p = malloc((size_t) len)) == NULL)
            goto cleanup;
        krb5_cas[i]->subjectName.data = (char *)p;
        i2d_X509_NAME(xn, &p);
        krb5_cas[i]->subjectName.length = len;

        /* fill-in issuerAndSerialNumber */
        krb5_cas[i]->issuerAndSerialNumber.length = 0;
        krb5_cas[i]->issuerAndSerialNumber.magic = 0;
        krb5_cas[i]->issuerAndSerialNumber.data = NULL;

#ifdef LONGHORN_BETA_COMPAT
        if (longhorn == 0) { /* XXX Longhorn doesn't like this */
#endif
            is = PKCS7_ISSUER_AND_SERIAL_new();
            X509_NAME_set(&is->issuer, X509_get_issuer_name(x));
            M_ASN1_INTEGER_free(is->serial);
            is->serial = M_ASN1_INTEGER_dup(X509_get_serialNumber(x));
            len = i2d_PKCS7_ISSUER_AND_SERIAL(is, NULL);
            if ((p = malloc((size_t) len)) == NULL)
                goto cleanup;
            krb5_cas[i]->issuerAndSerialNumber.data = (char *)p;
            i2d_PKCS7_ISSUER_AND_SERIAL(is, &p);
            krb5_cas[i]->issuerAndSerialNumber.length = len;
#ifdef LONGHORN_BETA_COMPAT
        }
#endif

        /* fill-in subjectKeyIdentifier */
        krb5_cas[i]->subjectKeyIdentifier.length = 0;
        krb5_cas[i]->subjectKeyIdentifier.magic = 0;
        krb5_cas[i]->subjectKeyIdentifier.data = NULL;


#ifdef LONGHORN_BETA_COMPAT
        if (longhorn == 0) {    /* XXX Longhorn doesn't like this */
#endif
            if (X509_get_ext_by_NID(x, NID_subject_key_identifier, -1) >= 0) {
                ASN1_OCTET_STRING *ikeyid = NULL;

                if ((ikeyid = X509_get_ext_d2i(x, NID_subject_key_identifier, NULL,
                                               NULL))) {
                    len = i2d_ASN1_OCTET_STRING(ikeyid, NULL);
                    if ((p = malloc((size_t) len)) == NULL)
                        goto cleanup;
                    krb5_cas[i]->subjectKeyIdentifier.data = (char *)p;
                    i2d_ASN1_OCTET_STRING(ikeyid, &p);
                    krb5_cas[i]->subjectKeyIdentifier.length = len;
                }
                if (ikeyid != NULL)
                    ASN1_OCTET_STRING_free(ikeyid);
            }
#ifdef LONGHORN_BETA_COMPAT
        }
#endif
        if (is != NULL) {
            if (is->issuer != NULL)
                X509_NAME_free(is->issuer);
            if (is->serial != NULL)
                ASN1_INTEGER_free(is->serial);
            free(is);
        }
    }

    *ids = krb5_cas;

    retval = 0;
cleanup:
    if (retval)
        free_krb5_external_principal_identifier(&krb5_cas);

    return retval;
}
