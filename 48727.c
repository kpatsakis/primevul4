xdr_krb5_ui_2(XDR *xdrs, krb5_ui_2 *objp)
{
    unsigned int tmp;

    tmp = (unsigned int) *objp;

    if (!xdr_u_int(xdrs, &tmp))
	return(FALSE);

    *objp = (krb5_ui_2) tmp;

    return(TRUE);
}
