xdr_cprinc_arg(XDR *xdrs, cprinc_arg *objp)
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
	if (!xdr_nullstring(xdrs, &objp->passwd)) {
		return (FALSE);
	}
	return (TRUE);
}
