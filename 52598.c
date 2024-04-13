kdc_handle_protected_negotiation(krb5_context context,
                                 krb5_data *req_pkt, krb5_kdc_req *request,
                                 const krb5_keyblock *reply_key,
                                 krb5_pa_data ***out_enc_padata)
{
    krb5_error_code retval = 0;
    krb5_checksum checksum;
    krb5_data *out = NULL;
    krb5_pa_data pa, *pa_in;
    pa_in = krb5int_find_pa_data(context, request->padata,
                                 KRB5_ENCPADATA_REQ_ENC_PA_REP);
    if (pa_in == NULL)
        return 0;
    pa.magic = KV5M_PA_DATA;
    pa.pa_type = KRB5_ENCPADATA_REQ_ENC_PA_REP;
    memset(&checksum, 0, sizeof(checksum));
    retval = krb5_c_make_checksum(context,0, reply_key,
                                  KRB5_KEYUSAGE_AS_REQ, req_pkt, &checksum);
    if (retval != 0)
        goto cleanup;
    retval = encode_krb5_checksum(&checksum, &out);
    if (retval != 0)
        goto cleanup;
    pa.contents = (krb5_octet *) out->data;
    pa.length = out->length;
    retval = add_pa_data_element(context, &pa, out_enc_padata, FALSE);
    if (retval)
        goto cleanup;
    out->data = NULL;
    pa.magic = KV5M_PA_DATA;
    pa.pa_type = KRB5_PADATA_FX_FAST;
    pa.length = 0;
    pa.contents = NULL;
    retval = add_pa_data_element(context, &pa, out_enc_padata, FALSE);
cleanup:
    if (checksum.contents)
        krb5_free_checksum_contents(context, &checksum);
    if (out != NULL)
        krb5_free_data(context, out);
    return retval;
}
