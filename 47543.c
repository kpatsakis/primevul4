iakerb_verify_finished(krb5_context context,
                       krb5_key key,
                       const krb5_data *conv,
                       const krb5_data *finished)
{
    krb5_error_code code;
    krb5_iakerb_finished *iaf;
    krb5_boolean valid = FALSE;

    if (key == NULL)
        return KRB5KDC_ERR_NULL_KEY;

    code = decode_krb5_iakerb_finished(finished, &iaf);
    if (code != 0)
        return code;

    code = krb5_k_verify_checksum(context, key, KRB5_KEYUSAGE_IAKERB_FINISHED,
                                  conv, &iaf->checksum, &valid);
    if (code == 0 && valid == FALSE)
        code = KRB5KRB_AP_ERR_BAD_INTEGRITY;

    krb5_free_iakerb_finished(context, iaf);

    return code;
}
