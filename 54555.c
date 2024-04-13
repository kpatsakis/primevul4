pa_pkinit_validate(kdc_request_t r, const PA_DATA *pa)
{
    pk_client_params *pkp = NULL;
    char *client_cert = NULL;
    krb5_error_code ret;

    ret = _kdc_pk_rd_padata(r->context, r->config, &r->req, pa, r->client, &pkp);
    if (ret || pkp == NULL) {
	ret = KRB5KRB_AP_ERR_BAD_INTEGRITY;
	_kdc_r_log(r, 5, "Failed to decode PKINIT PA-DATA -- %s",
		   r->client_name);
	goto out;
    }
    
    ret = _kdc_pk_check_client(r->context,
			       r->config,
			       r->clientdb, 
			       r->client,
			       pkp,
			       &client_cert);
    if (ret) {
	_kdc_set_e_text(r, "PKINIT certificate not allowed to "
			"impersonate principal");
	goto out;
    }

    _kdc_r_log(r, 0, "PKINIT pre-authentication succeeded -- %s using %s",
	       r->client_name, client_cert);
    free(client_cert);

    ret = _kdc_pk_mk_pa_reply(r->context, r->config, pkp, r->client,
			      r->sessionetype, &r->req, &r->request,
			      &r->reply_key, &r->session_key, &r->outpadata);
    if (ret) {
	_kdc_set_e_text(r, "Failed to build PK-INIT reply");
	goto out;
    }
#if 0
    ret = _kdc_add_inital_verified_cas(r->context, r->config,
				       pkp, &r->et);
#endif
 out:
    if (pkp)
	_kdc_pk_free_client_param(r->context, pkp);

    return ret;
}
