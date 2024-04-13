_xdr_kadm5_principal_ent_rec(XDR *xdrs, kadm5_principal_ent_rec *objp,
			     int v)
{
	unsigned int n;

	if (!xdr_krb5_principal(xdrs, &objp->principal)) {
		return (FALSE);
	}
	if (!xdr_krb5_timestamp(xdrs, &objp->princ_expire_time)) {
		return (FALSE);
	}
	if (!xdr_krb5_timestamp(xdrs, &objp->last_pwd_change)) {
		return (FALSE);
	}
	if (!xdr_krb5_timestamp(xdrs, &objp->pw_expiration)) {
		return (FALSE);
	}
	if (!xdr_krb5_deltat(xdrs, &objp->max_life)) {
		return (FALSE);
	}
	if (!xdr_nulltype(xdrs, (void **) &objp->mod_name,
			  xdr_krb5_principal)) {
		return (FALSE);
	}
	if (!xdr_krb5_timestamp(xdrs, &objp->mod_date)) {
		return (FALSE);
	}
	if (!xdr_krb5_flags(xdrs, &objp->attributes)) {
		return (FALSE);
	}
	if (!xdr_krb5_kvno(xdrs, &objp->kvno)) {
		return (FALSE);
	}
	if (!xdr_krb5_kvno(xdrs, &objp->mkvno)) {
		return (FALSE);
	}
	if (!xdr_nullstring(xdrs, &objp->policy)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->aux_attributes)) {
		return (FALSE);
	}
	if (!xdr_krb5_deltat(xdrs, &objp->max_renewable_life)) {
		return (FALSE);
	}
	if (!xdr_krb5_timestamp(xdrs, &objp->last_success)) {
		return (FALSE);
	}
	if (!xdr_krb5_timestamp(xdrs, &objp->last_failed)) {
		return (FALSE);
	}
	if (!xdr_krb5_kvno(xdrs, &objp->fail_auth_count)) {
		return (FALSE);
	}
	if (!xdr_krb5_int16(xdrs, &objp->n_key_data)) {
		return (FALSE);
	}
	if (!xdr_krb5_int16(xdrs, &objp->n_tl_data)) {
		return (FALSE);
	}
	if (!xdr_nulltype(xdrs, (void **) &objp->tl_data,
			  xdr_krb5_tl_data)) {
		return FALSE;
	}
	n = objp->n_key_data;
	if (!xdr_array(xdrs, (caddr_t *) &objp->key_data,
		       &n, ~0, sizeof(krb5_key_data),
		       xdr_krb5_key_data_nocontents)) {
		return (FALSE);
	}

	return (TRUE);
}
