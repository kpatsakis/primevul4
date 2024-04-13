verify_s4u_x509_user_checksum(krb5_context context,
                              krb5_keyblock *key,
                              krb5_data *req_data,
                              krb5_int32 kdc_req_nonce,
                              krb5_pa_s4u_x509_user *req)
{
    krb5_error_code             code;
    krb5_data                   scratch;
    krb5_boolean                valid = FALSE;

    if (enctype_requires_etype_info_2(key->enctype) &&
        !krb5_c_is_keyed_cksum(req->cksum.checksum_type))
        return KRB5KRB_AP_ERR_INAPP_CKSUM;

    if (req->user_id.nonce != kdc_req_nonce)
        return KRB5KRB_AP_ERR_MODIFIED;

    /*
     * Verify checksum over the encoded userid. If that fails,
     * re-encode, and verify that. This is similar to the
     * behaviour in kdc_process_tgs_req().
     */
    if (fetch_asn1_field((unsigned char *)req_data->data, 1, 0, &scratch) < 0)
        return ASN1_PARSE_ERROR;

    code = krb5_c_verify_checksum(context,
                                  key,
                                  KRB5_KEYUSAGE_PA_S4U_X509_USER_REQUEST,
                                  &scratch,
                                  &req->cksum,
                                  &valid);
    if (code != 0)
        return code;

    if (valid == FALSE) {
        krb5_data *data;

        code = encode_krb5_s4u_userid(&req->user_id, &data);
        if (code != 0)
            return code;

        code = krb5_c_verify_checksum(context,
                                      key,
                                      KRB5_KEYUSAGE_PA_S4U_X509_USER_REQUEST,
                                      data,
                                      &req->cksum,
                                      &valid);

        krb5_free_data(context, data);

        if (code != 0)
            return code;
    }

    return valid ? 0 : KRB5KRB_AP_ERR_MODIFIED;
}
