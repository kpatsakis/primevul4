xdr_rprinc_arg(XDR *xdrs, rprinc_arg *objp)
{
	if (!xdr_ui_4(xdrs, &objp->api_version)) {
		return (FALSE);
	}
	if (!xdr_krb5_principal(xdrs, &objp->src)) {
		return (FALSE);
	}
	if (!xdr_krb5_principal(xdrs, &objp->dest)) {
		return (FALSE);
	}
	return (TRUE);
}
