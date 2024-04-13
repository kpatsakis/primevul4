xdr_krb5_string_attr(XDR *xdrs, krb5_string_attr *objp)
{
	if (!xdr_nullstring(xdrs, &objp->key))
		return FALSE;
	if (!xdr_nullstring(xdrs, &objp->value))
		return FALSE;
	if (xdrs->x_op == XDR_DECODE &&
	    (objp->key == NULL || objp->value == NULL))
		return FALSE;
	return TRUE;
}
