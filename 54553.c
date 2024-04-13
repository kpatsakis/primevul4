pa_enc_chal_validate(kdc_request_t r, const PA_DATA *pa)
{
    krb5_data pepper1, pepper2, ts_data;
    KDC_REQ_BODY *b = &r->req.req_body;
    int invalidPassword = 0;
    EncryptedData enc_data;
    krb5_enctype aenctype;
    krb5_error_code ret;
    struct Key *k;
    size_t size;
    int i;

    heim_assert(r->armor_crypto != NULL, "ENC-CHAL called for non FAST");
    
    if (_kdc_is_anon_request(b)) {
	ret = KRB5KRB_AP_ERR_BAD_INTEGRITY;
	kdc_log(r->context, r->config, 0, "ENC-CHALL doesn't support anon");
	return ret;
    }

    ret = decode_EncryptedData(pa->padata_value.data,
			       pa->padata_value.length,
			       &enc_data,
			       &size);
    if (ret) {
	ret = KRB5KRB_AP_ERR_BAD_INTEGRITY;
	_kdc_r_log(r, 5, "Failed to decode PA-DATA -- %s",
		   r->client_name);
	return ret;
    }

    pepper1.data = "clientchallengearmor";
    pepper1.length = strlen(pepper1.data);
    pepper2.data = "challengelongterm";
    pepper2.length = strlen(pepper2.data);

    krb5_crypto_getenctype(r->context, r->armor_crypto, &aenctype);

    for (i = 0; i < r->client->entry.keys.len; i++) {
	krb5_crypto challangecrypto, longtermcrypto;
	krb5_keyblock challangekey;
	PA_ENC_TS_ENC p;

	k = &r->client->entry.keys.val[i];
	
	ret = krb5_crypto_init(r->context, &k->key, 0, &longtermcrypto);
	if (ret)
	    continue;			
	
	ret = krb5_crypto_fx_cf2(r->context, r->armor_crypto, longtermcrypto,
				 &pepper1, &pepper2, aenctype,
				 &challangekey);
	krb5_crypto_destroy(r->context, longtermcrypto);
	if (ret)
	    continue;
	
	ret = krb5_crypto_init(r->context, &challangekey, 0,
			       &challangecrypto);
	if (ret)
	    continue;
	
	ret = krb5_decrypt_EncryptedData(r->context, challangecrypto,
					 KRB5_KU_ENC_CHALLENGE_CLIENT,
					 &enc_data,
					 &ts_data);
	if (ret) {
	    const char *msg = krb5_get_error_message(r->context, ret);
	    krb5_error_code ret2;
	    char *str = NULL;

	    invalidPassword = 1;

	    ret2 = krb5_enctype_to_string(r->context, k->key.keytype, &str);
	    if (ret2)
		str = NULL;
	    _kdc_r_log(r, 5, "Failed to decrypt ENC-CHAL -- %s "
		       "(enctype %s) error %s",
		       r->client_name, str ? str : "unknown enctype", msg);
	    krb5_free_error_message(r->context, msg);
	    free(str);

	    continue;
	}
	
	ret = decode_PA_ENC_TS_ENC(ts_data.data,
				   ts_data.length,
				   &p,
				   &size);
	krb5_data_free(&ts_data);
	if(ret){
	    krb5_crypto_destroy(r->context, challangecrypto);
	    ret = KRB5KDC_ERR_PREAUTH_FAILED;
	    _kdc_r_log(r, 5, "Failed to decode PA-ENC-TS_ENC -- %s",
		       r->client_name);
	    continue;
	}

	if (labs(kdc_time - p.patimestamp) > r->context->max_skew) {
	    char client_time[100];

	    krb5_crypto_destroy(r->context, challangecrypto);

	    krb5_format_time(r->context, p.patimestamp,
			     client_time, sizeof(client_time), TRUE);

	    ret = KRB5KRB_AP_ERR_SKEW;
	    _kdc_r_log(r, 0, "Too large time skew, "
		       "client time %s is out by %u > %u seconds -- %s",
		       client_time,
		       (unsigned)labs(kdc_time - p.patimestamp),
		       r->context->max_skew,
		       r->client_name);

	    free_PA_ENC_TS_ENC(&p);
	    goto out;
	}

	free_PA_ENC_TS_ENC(&p);

	ret = make_pa_enc_challange(r->context, &r->outpadata,
				    challangecrypto);
	krb5_crypto_destroy(r->context, challangecrypto);
	if (ret)
	    goto out;
					    
	set_salt_padata(&r->outpadata, k->salt);
	krb5_free_keyblock_contents(r->context,  &r->reply_key);
	ret = krb5_copy_keyblock_contents(r->context, &k->key, &r->reply_key);
	if (ret)
	    goto out;

	/*
	 * Success
	 */
	if (r->clientdb->hdb_auth_status)
	    r->clientdb->hdb_auth_status(r->context, r->clientdb, r->client,
					 HDB_AUTH_SUCCESS);
	goto out;
    }

    if (invalidPassword && r->clientdb->hdb_auth_status) {
	r->clientdb->hdb_auth_status(r->context, r->clientdb, r->client,
				     HDB_AUTH_WRONG_PASSWORD);
	ret = KRB5KDC_ERR_PREAUTH_FAILED;
    }
 out:
    free_EncryptedData(&enc_data);

    return ret;
}
