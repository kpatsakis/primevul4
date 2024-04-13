xdr_chrand_arg(XDR *xdrs, chrand_arg *objp)
{
	if (!xdr_ui_4(xdrs, &objp->api_version)) {
		return (FALSE);
	}
	if (!xdr_krb5_principal(xdrs, &objp->princ)) {
		return (FALSE);
	}
	return (TRUE);
}
