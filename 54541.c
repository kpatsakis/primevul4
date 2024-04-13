generate_pac(kdc_request_t r, Key *skey)
{
    krb5_error_code ret;
    krb5_pac p = NULL;
    krb5_data data;

    ret = _kdc_pac_generate(r->context, r->client, &p);
    if (ret) {
	_kdc_r_log(r, 0, "PAC generation failed for -- %s",
		   r->client_name);
	return ret;
    }
    if (p == NULL)
	return 0;

    ret = _krb5_pac_sign(r->context, p, r->et.authtime,
			 r->client->entry.principal,
			 &skey->key, /* Server key */
			 &skey->key, /* FIXME: should be krbtgt key */
			 &data);
    krb5_pac_free(r->context, p);
    if (ret) {
	_kdc_r_log(r, 0, "PAC signing failed for -- %s",
		   r->client_name);
	return ret;
    }
    
    ret = _kdc_tkt_add_if_relevant_ad(r->context, &r->et,
				      KRB5_AUTHDATA_WIN2K_PAC,
				      &data);
    krb5_data_free(&data);

    return ret;
}
