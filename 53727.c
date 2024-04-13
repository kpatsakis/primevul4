xdr_krb5_kvno(XDR *xdrs, krb5_kvno *objp)
{
	return xdr_u_int(xdrs, objp);
}
