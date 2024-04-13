xdr_dpol_arg(XDR *xdrs, dpol_arg *objp)
{
	if (!xdr_ui_4(xdrs, &objp->api_version)) {
		return (FALSE);
	}
	if (!xdr_nullstring(xdrs, &objp->name)) {
		return (FALSE);
	}
	return (TRUE);
}
