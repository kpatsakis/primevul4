xdr_chpass_arg(XDR *xdrs, chpass_arg *objp)
{
	if (!xdr_ui_4(xdrs, &objp->api_version)) {
		return (FALSE);
	}
	if (!xdr_krb5_principal(xdrs, &objp->princ)) {
		return (FALSE);
	}
	if (!xdr_nullstring(xdrs, &objp->pass)) {
		return (FALSE);
	}
	return (TRUE);
}
