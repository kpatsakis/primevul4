xdr_gprinc_ret(XDR *xdrs, gprinc_ret *objp)
{
	if (!xdr_ui_4(xdrs, &objp->api_version)) {
		return (FALSE);
	}
	if (!xdr_kadm5_ret_t(xdrs, &objp->code)) {
		return (FALSE);
	}
	if(objp->code == KADM5_OK)  {
		if (!_xdr_kadm5_principal_ent_rec(xdrs, &objp->rec,
						  objp->api_version)) {
			return (FALSE);
		}
	}

	return (TRUE);
}
