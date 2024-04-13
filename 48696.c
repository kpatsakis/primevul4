xdr_chrand3_arg(XDR *xdrs, chrand3_arg *objp)
{
	if (!xdr_ui_4(xdrs, &objp->api_version)) {
		return (FALSE);
	}
	if (!xdr_krb5_principal(xdrs, &objp->princ)) {
		return (FALSE);
	}
	if (!xdr_krb5_boolean(xdrs, &objp->keepold)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (caddr_t *)&objp->ks_tuple,
		       (unsigned int*)&objp->n_ks_tuple, ~0,
		       sizeof(krb5_key_salt_tuple),
		       xdr_krb5_key_salt_tuple)) {
		return (FALSE);
	}
	return (TRUE);
}
