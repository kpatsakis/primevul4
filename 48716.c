static bool_t xdr_krb5_boolean(XDR *xdrs, krb5_boolean *kbool)
{
	bool_t val;

	switch (xdrs->x_op) {
	case XDR_DECODE:
	     if (!xdr_bool(xdrs, &val))
		     return FALSE;

	     *kbool = (val == FALSE) ? FALSE : TRUE;
	     return TRUE;

	case XDR_ENCODE:
	     val = *kbool ? TRUE : FALSE;
	     return xdr_bool(xdrs, &val);

	case XDR_FREE:
	     return TRUE;
	}

	return FALSE;
}
