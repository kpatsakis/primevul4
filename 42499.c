return_pkinit_kx(krb5_context context, krb5_kdc_req *request,
                 krb5_kdc_rep *reply, krb5_keyblock *encrypting_key,
                 krb5_pa_data **out_padata)
{
    krb5_error_code ret = 0;
    krb5_keyblock *session = reply->ticket->enc_part2->session;
    krb5_keyblock *new_session = NULL;
    krb5_pa_data *pa = NULL;
    krb5_enc_data enc;
    krb5_data *scratch = NULL;

    *out_padata = NULL;
    enc.ciphertext.data = NULL;
    if (!krb5_principal_compare(context, request->client,
                                krb5_anonymous_principal()))
        return 0;
    /*
     * The KDC contribution key needs to be a fresh key of an enctype supported
     * by the client and server. The existing session key meets these
     * requirements so we use it.
     */
    ret = krb5_c_fx_cf2_simple(context, session, "PKINIT",
                               encrypting_key, "KEYEXCHANGE",
                               &new_session);
    if (ret)
        goto cleanup;
    ret = encode_krb5_encryption_key( session, &scratch);
    if (ret)
        goto cleanup;
    ret = krb5_encrypt_helper(context, encrypting_key,
                              KRB5_KEYUSAGE_PA_PKINIT_KX, scratch, &enc);
    if (ret)
        goto cleanup;
    memset(scratch->data, 0, scratch->length);
    krb5_free_data(context, scratch);
    scratch = NULL;
    ret = encode_krb5_enc_data(&enc, &scratch);
    if (ret)
        goto cleanup;
    pa = malloc(sizeof(krb5_pa_data));
    if (pa == NULL) {
        ret = ENOMEM;
        goto cleanup;
    }
    pa->pa_type = KRB5_PADATA_PKINIT_KX;
    pa->length = scratch->length;
    pa->contents = (krb5_octet *) scratch->data;
    *out_padata = pa;
    scratch->data = NULL;
    memset(session->contents, 0, session->length);
    krb5_free_keyblock_contents(context, session);
    *session = *new_session;
    new_session->contents = NULL;
cleanup:
    krb5_free_data_contents(context, &enc.ciphertext);
    krb5_free_keyblock(context, new_session);
    krb5_free_data(context, scratch);
    return ret;
}
