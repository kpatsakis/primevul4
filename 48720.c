xdr_krb5_int16(XDR *xdrs, krb5_int16 *objp)
{
    int tmp;

    tmp = (int) *objp;

    if (!xdr_int(xdrs, &tmp))
	return(FALSE);

    *objp = (krb5_int16) tmp;

    return(TRUE);
}
