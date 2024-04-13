verify_for_user_checksum(krb5_context context,
                         krb5_keyblock *key,
                         krb5_pa_for_user *req)
{
    krb5_error_code             code;
    int                         i;
    krb5_int32                  name_type;
    char                        *p;
    krb5_data                   data;
    krb5_boolean                valid = FALSE;

    if (!krb5_c_is_keyed_cksum(req->cksum.checksum_type)) {
        return KRB5KRB_AP_ERR_INAPP_CKSUM;
    }

    /*
     * Checksum is over name type and string components of
     * client principal name and auth_package.
     */
    data.length = 4;
    for (i = 0; i < krb5_princ_size(context, req->user); i++) {
        data.length += krb5_princ_component(context, req->user, i)->length;
    }
    data.length += krb5_princ_realm(context, req->user)->length;
    data.length += req->auth_package.length;

    p = data.data = malloc(data.length);
    if (data.data == NULL) {
        return ENOMEM;
    }

    name_type = krb5_princ_type(context, req->user);
    p[0] = (name_type >> 0 ) & 0xFF;
    p[1] = (name_type >> 8 ) & 0xFF;
    p[2] = (name_type >> 16) & 0xFF;
    p[3] = (name_type >> 24) & 0xFF;
    p += 4;

    for (i = 0; i < krb5_princ_size(context, req->user); i++) {
        if (krb5_princ_component(context, req->user, i)->length > 0) {
            memcpy(p, krb5_princ_component(context, req->user, i)->data,
                   krb5_princ_component(context, req->user, i)->length);
        }
        p += krb5_princ_component(context, req->user, i)->length;
    }

    if (krb5_princ_realm(context, req->user)->length > 0) {
        memcpy(p, krb5_princ_realm(context, req->user)->data,
               krb5_princ_realm(context, req->user)->length);
    }
    p += krb5_princ_realm(context, req->user)->length;

    if (req->auth_package.length > 0)
        memcpy(p, req->auth_package.data, req->auth_package.length);
    p += req->auth_package.length;

    code = krb5_c_verify_checksum(context,
                                  key,
                                  KRB5_KEYUSAGE_APP_DATA_CKSUM,
                                  &data,
                                  &req->cksum,
                                  &valid);

    if (code == 0 && valid == FALSE)
        code = KRB5KRB_AP_ERR_MODIFIED;

    free(data.data);

    return code;
}
