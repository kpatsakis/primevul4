xdr_gprinc_arg(XDR *xdrs, gprinc_arg *objp)
{
	if (!xdr_ui_4(xdrs, &objp->api_version)) {
		return (FALSE);
	}
	if (!xdr_krb5_principal(xdrs, &objp->princ)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->mask)) {
	     return FALSE;
	}

	return (TRUE);
}
