xdr_cprinc3_arg(XDR *xdrs, cprinc3_arg *objp)
{
	if (!xdr_ui_4(xdrs, &objp->api_version)) {
		return (FALSE);
	}
	if (!_xdr_kadm5_principal_ent_rec(xdrs, &objp->rec,
					  objp->api_version)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->mask)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (caddr_t *)&objp->ks_tuple,
		       (unsigned int *)&objp->n_ks_tuple, ~0,
		       sizeof(krb5_key_salt_tuple),
		       xdr_krb5_key_salt_tuple)) {
		return (FALSE);
	}
	if (!xdr_nullstring(xdrs, &objp->passwd)) {
		return (FALSE);
	}
	return (TRUE);
}
