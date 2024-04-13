kdc_make_s4u2self_rep(krb5_context context,
                      krb5_keyblock *tgs_subkey,
                      krb5_keyblock *tgs_session,
                      krb5_pa_s4u_x509_user *req_s4u_user,
                      krb5_kdc_rep *reply,
                      krb5_enc_kdc_rep_part *reply_encpart)
{
    krb5_error_code             code;
    krb5_data                   *data = NULL;
    krb5_pa_s4u_x509_user       rep_s4u_user;
    krb5_pa_data                padata;
    krb5_enctype                enctype;
    krb5_keyusage               usage;

    memset(&rep_s4u_user, 0, sizeof(rep_s4u_user));

    rep_s4u_user.user_id.nonce   = req_s4u_user->user_id.nonce;
    rep_s4u_user.user_id.user    = req_s4u_user->user_id.user;
    rep_s4u_user.user_id.options =
        req_s4u_user->user_id.options & KRB5_S4U_OPTS_USE_REPLY_KEY_USAGE;

    code = encode_krb5_s4u_userid(&rep_s4u_user.user_id, &data);
    if (code != 0)
        goto cleanup;

    if (req_s4u_user->user_id.options & KRB5_S4U_OPTS_USE_REPLY_KEY_USAGE)
        usage = KRB5_KEYUSAGE_PA_S4U_X509_USER_REPLY;
    else
        usage = KRB5_KEYUSAGE_PA_S4U_X509_USER_REQUEST;

    code = krb5_c_make_checksum(context, req_s4u_user->cksum.checksum_type,
                                tgs_subkey != NULL ? tgs_subkey : tgs_session,
                                usage, data,
                                &rep_s4u_user.cksum);
    if (code != 0)
        goto cleanup;

    krb5_free_data(context, data);
    data = NULL;

    code = encode_krb5_pa_s4u_x509_user(&rep_s4u_user, &data);
    if (code != 0)
        goto cleanup;

    padata.magic = KV5M_PA_DATA;
    padata.pa_type = KRB5_PADATA_S4U_X509_USER;
    padata.length = data->length;
    padata.contents = (krb5_octet *)data->data;

    code = add_pa_data_element(context, &padata, &reply->padata, FALSE);
    if (code != 0)
        goto cleanup;

    free(data);
    data = NULL;

    if (tgs_subkey != NULL)
        enctype = tgs_subkey->enctype;
    else
        enctype = tgs_session->enctype;

    /*
     * Owing to a bug in Windows, unkeyed checksums were used for older
     * enctypes, including rc4-hmac. A forthcoming workaround for this
     * includes the checksum bytes in the encrypted padata.
     */
    if ((req_s4u_user->user_id.options & KRB5_S4U_OPTS_USE_REPLY_KEY_USAGE) &&
        enctype_requires_etype_info_2(enctype) == FALSE) {
        padata.length = req_s4u_user->cksum.length +
            rep_s4u_user.cksum.length;
        padata.contents = malloc(padata.length);
        if (padata.contents == NULL) {
            code = ENOMEM;
            goto cleanup;
        }

        memcpy(padata.contents,
               req_s4u_user->cksum.contents,
               req_s4u_user->cksum.length);
        memcpy(&padata.contents[req_s4u_user->cksum.length],
               rep_s4u_user.cksum.contents,
               rep_s4u_user.cksum.length);

        code = add_pa_data_element(context,&padata,
                                   &reply_encpart->enc_padata, FALSE);
        if (code != 0) {
            free(padata.contents);
            goto cleanup;
        }
    }

cleanup:
    if (rep_s4u_user.cksum.contents != NULL)
        krb5_free_checksum_contents(context, &rep_s4u_user.cksum);
    krb5_free_data(context, data);

    return code;
}
