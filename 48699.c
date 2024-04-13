xdr_cpol_arg(XDR *xdrs, cpol_arg *objp)
{
	if (!xdr_ui_4(xdrs, &objp->api_version)) {
		return (FALSE);
	}
	if (!_xdr_kadm5_policy_ent_rec(xdrs, &objp->rec,
				       objp->api_version)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->mask)) {
		return (FALSE);
	}
	return (TRUE);
}
