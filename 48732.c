xdr_purgekeys_arg(XDR *xdrs, purgekeys_arg *objp)
{
	if (!xdr_ui_4(xdrs, &objp->api_version)) {
		return (FALSE);
	}
	if (!xdr_krb5_principal(xdrs, &objp->princ)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->keepkvno)) {
	     return FALSE;
	}

	return (TRUE);
}
