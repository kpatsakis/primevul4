xdr_kadm5_ret_t(XDR *xdrs, kadm5_ret_t *objp)
{
	uint32_t tmp;

	if (xdrs->x_op == XDR_ENCODE)
		tmp = (uint32_t) *objp;

	if (!xdr_u_int32(xdrs, &tmp))
		return (FALSE);

	if (xdrs->x_op == XDR_DECODE)
		*objp = (kadm5_ret_t) tmp;

	return (TRUE);
}
