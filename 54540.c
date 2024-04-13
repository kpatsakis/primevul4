add_enc_pa_rep(kdc_request_t r)
{
    krb5_error_code ret;
    krb5_crypto crypto;
    Checksum checksum;
    krb5_data cdata;
    size_t len;

    ret = krb5_crypto_init(r->context, &r->reply_key, 0, &crypto);
    if (ret)
	return ret;

    ret = krb5_create_checksum(r->context, crypto,
			       KRB5_KU_AS_REQ, 0,
			       r->request.data, r->request.length,
			       &checksum);
    krb5_crypto_destroy(r->context, crypto);
    if (ret)
	return ret;

    ASN1_MALLOC_ENCODE(Checksum, cdata.data, cdata.length,
		       &checksum, &len, ret);
    free_Checksum(&checksum);
    if (ret)
	return ret;
    heim_assert(cdata.length == len, "ASN.1 internal error");

    if (r->ek.encrypted_pa_data == NULL) {
	ALLOC(r->ek.encrypted_pa_data);
	if (r->ek.encrypted_pa_data == NULL)
	    return ENOMEM;
    }
    ret = krb5_padata_add(r->context, r->ek.encrypted_pa_data,
			  KRB5_PADATA_REQ_ENC_PA_REP, cdata.data, cdata.length);
    if (ret)
	return ret;
    
    return krb5_padata_add(r->context, r->ek.encrypted_pa_data,
			   KRB5_PADATA_FX_FAST, NULL, 0);
}
