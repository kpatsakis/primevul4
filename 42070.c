create_contentinfo(krb5_context context,
                   pkinit_plg_crypto_context plg_crypto_context,
                   ASN1_OBJECT *oid, unsigned char *data, size_t data_len,
                   PKCS7 **out_p7)
{
    krb5_error_code retval = EINVAL;
    PKCS7 *inner_p7;
    ASN1_TYPE *pkinit_data = NULL;

    *out_p7 = NULL;
    if ((inner_p7 = PKCS7_new()) == NULL)
        goto cleanup;
    if ((pkinit_data = ASN1_TYPE_new()) == NULL)
        goto cleanup;
    pkinit_data->type = V_ASN1_OCTET_STRING;
    if ((pkinit_data->value.octet_string = ASN1_OCTET_STRING_new()) == NULL)
        goto cleanup;
    if (!ASN1_OCTET_STRING_set(pkinit_data->value.octet_string,
                               (unsigned char *) data, data_len)) {
        unsigned long err = ERR_peek_error();
        retval = KRB5KDC_ERR_PREAUTH_FAILED;
        krb5_set_error_message(context, retval, "%s\n",
                               ERR_error_string(err, NULL));
        pkiDebug("failed to add pkcs7 data\n");
        goto cleanup;
    }
    if (!PKCS7_set0_type_other(inner_p7, OBJ_obj2nid(oid), pkinit_data))
        goto cleanup;
    retval = 0;
    *out_p7 = inner_p7;
    inner_p7 = NULL;
    pkinit_data = NULL;
cleanup:
    if (inner_p7)
        PKCS7_free(inner_p7);
    if (pkinit_data)
        ASN1_TYPE_free(pkinit_data);
    return retval;
}
