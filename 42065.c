cms_contentinfo_create(krb5_context context,                          /* IN */
                       pkinit_plg_crypto_context plg_cryptoctx,       /* IN */
                       pkinit_req_crypto_context req_cryptoctx,       /* IN */
                       pkinit_identity_crypto_context id_cryptoctx,   /* IN */
                       int cms_msg_type,
                       unsigned char *data, unsigned int data_len,
                       unsigned char **out_data, unsigned int *out_data_len)
{
    krb5_error_code retval = ENOMEM;
    ASN1_OBJECT *oid = NULL;
    PKCS7 *p7 = NULL;
    unsigned char *p;

    /* Pick the correct oid for the eContentInfo. */
    oid = pkinit_pkcs7type2oid(plg_cryptoctx, cms_msg_type);
    if (oid == NULL)
        goto cleanup;
    retval = create_contentinfo(context, plg_cryptoctx, oid,
                                data, data_len, &p7);
    if (retval != 0)
        goto cleanup;
    *out_data_len = i2d_PKCS7(p7, NULL);
    if (!(*out_data_len)) {
        unsigned long err = ERR_peek_error();
        retval = KRB5KDC_ERR_PREAUTH_FAILED;
        krb5_set_error_message(context, retval, "%s\n",
                               ERR_error_string(err, NULL));
        pkiDebug("failed to der encode pkcs7\n");
        goto cleanup;
    }
    retval = ENOMEM;
    if ((p = *out_data = malloc(*out_data_len)) == NULL)
        goto cleanup;

    /* DER encode PKCS7 data */
    retval = i2d_PKCS7(p7, &p);
    if (!retval) {
        unsigned long err = ERR_peek_error();
        retval = KRB5KDC_ERR_PREAUTH_FAILED;
        krb5_set_error_message(context, retval, "%s\n",
                               ERR_error_string(err, NULL));
        pkiDebug("failed to der encode pkcs7\n");
        goto cleanup;
    }
    retval = 0;
cleanup:
    if (p7)
        PKCS7_free(p7);
    if (oid)
        ASN1_OBJECT_free(oid);
    return retval;
}
