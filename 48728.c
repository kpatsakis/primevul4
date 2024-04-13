xdr_krb5_ui_4(XDR *xdrs, krb5_ui_4 *objp)
{
	if (!xdr_u_int32(xdrs, (uint32_t *) objp)) {
		return (FALSE);
	}
	return (TRUE);
}
