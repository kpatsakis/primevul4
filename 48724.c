xdr_krb5_salttype(XDR *xdrs, krb5_int32 *objp)
{
    if (!xdr_int32(xdrs, (int32_t *) objp))
	return FALSE;
    return TRUE;
}
