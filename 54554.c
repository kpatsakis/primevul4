pa_enc_ts_validate(kdc_request_t r, const PA_DATA *pa)
{
    EncryptedData enc_data;
    krb5_error_code ret;
    krb5_crypto crypto;
    krb5_data ts_data;
    PA_ENC_TS_ENC p;
    size_t len;
    Key *pa_key;
    char *str;
	
    if (_kdc_is_anon_request(&r->req.req_body)) {
	ret = KRB5KRB_AP_ERR_BAD_INTEGRITY;
	_kdc_set_e_text(r, "ENC-TS doesn't support anon");
	goto out;
    }

    ret = decode_EncryptedData(pa->padata_value.data,
			       pa->padata_value.length,
			       &enc_data,
			       &len);
    if (ret) {
	ret = KRB5KRB_AP_ERR_BAD_INTEGRITY;
	_kdc_r_log(r, 5, "Failed to decode PA-DATA -- %s",
		   r->client_name);
	goto out;
    }
	
    ret = hdb_enctype2key(r->context, &r->client->entry, NULL,
			  enc_data.etype, &pa_key);
    if(ret){
	char *estr;
	_kdc_set_e_text(r, "No key matching entype");
	ret = KRB5KDC_ERR_ETYPE_NOSUPP;
	if(krb5_enctype_to_string(r->context, enc_data.etype, &estr))
	    estr = NULL;
	if(estr == NULL)
	    _kdc_r_log(r, 5,
		       "No client key matching pa-data (%d) -- %s",
		       enc_data.etype, r->client_name);
	else
	    _kdc_r_log(r, 5,
		       "No client key matching pa-data (%s) -- %s",
		       estr, r->client_name);
	free(estr);
	free_EncryptedData(&enc_data);
	goto out;
    }

 try_next_key:
    ret = krb5_crypto_init(r->context, &pa_key->key, 0, &crypto);
    if (ret) {
	const char *msg = krb5_get_error_message(r->context, ret);
	_kdc_r_log(r, 0, "krb5_crypto_init failed: %s", msg);
	krb5_free_error_message(r->context, msg);
	free_EncryptedData(&enc_data);
	goto out;
    }

    ret = krb5_decrypt_EncryptedData (r->context,
				      crypto,
				      KRB5_KU_PA_ENC_TIMESTAMP,
				      &enc_data,
				      &ts_data);
    krb5_crypto_destroy(r->context, crypto);
    /*
     * Since the user might have several keys with the same
     * enctype but with diffrent salting, we need to try all
     * the keys with the same enctype.
     */
    if(ret){
	krb5_error_code ret2;
	const char *msg = krb5_get_error_message(r->context, ret);

	ret2 = krb5_enctype_to_string(r->context,
				      pa_key->key.keytype, &str);
	if (ret2)
	    str = NULL;
	_kdc_r_log(r, 5, "Failed to decrypt PA-DATA -- %s "
		   "(enctype %s) error %s",
		   r->client_name, str ? str : "unknown enctype", msg);
	krb5_free_error_message(r->context, msg);
	free(str);

	if(hdb_next_enctype2key(r->context, &r->client->entry, NULL,
				enc_data.etype, &pa_key) == 0)
	    goto try_next_key;

	free_EncryptedData(&enc_data);

	if (r->clientdb->hdb_auth_status)
	    r->clientdb->hdb_auth_status(r->context, r->clientdb, r->client,
					 HDB_AUTH_WRONG_PASSWORD);

	ret = KRB5KDC_ERR_PREAUTH_FAILED;
	goto out;
    }
    free_EncryptedData(&enc_data);
    ret = decode_PA_ENC_TS_ENC(ts_data.data,
			       ts_data.length,
			       &p,
			       &len);
    krb5_data_free(&ts_data);
    if(ret){
	ret = KRB5KDC_ERR_PREAUTH_FAILED;
	_kdc_r_log(r, 5, "Failed to decode PA-ENC-TS_ENC -- %s",
		   r->client_name);
	goto out;
    }
    if (labs(kdc_time - p.patimestamp) > r->context->max_skew) {
	char client_time[100];
		
	krb5_format_time(r->context, p.patimestamp,
			 client_time, sizeof(client_time), TRUE);

	ret = KRB5KRB_AP_ERR_SKEW;
	_kdc_r_log(r, 0, "Too large time skew, "
		   "client time %s is out by %u > %u seconds -- %s",
		   client_time,
		   (unsigned)labs(kdc_time - p.patimestamp),
		   r->context->max_skew,
		   r->client_name);

	/*
	 * The following is needed to make windows clients to
	 * retry using the timestamp in the error message, if
	 * there is a e_text, they become unhappy.
	 */
	r->e_text = NULL;
	free_PA_ENC_TS_ENC(&p);
	goto out;
    }
    free_PA_ENC_TS_ENC(&p);

    set_salt_padata(&r->outpadata, pa_key->salt);

    ret = krb5_copy_keyblock_contents(r->context, &pa_key->key, &r->reply_key);
    if (ret)
	return ret;

    ret = krb5_enctype_to_string(r->context, pa_key->key.keytype, &str);
    if (ret)
	str = NULL;
    _kdc_r_log(r, 2, "ENC-TS Pre-authentication succeeded -- %s using %s",
	       r->client_name, str ? str : "unknown enctype");
    free(str);

    ret = 0;

 out:

    return ret;
}
