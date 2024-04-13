bool_t xdr_getprivs_ret(XDR *xdrs, getprivs_ret *objp)
{
	if (!xdr_ui_4(xdrs, &objp->api_version)) {
		return (FALSE);
	}
     if (! xdr_kadm5_ret_t(xdrs, &objp->code) ||
	 ! xdr_long(xdrs, &objp->privs))
	  return FALSE;

     return TRUE;
}
