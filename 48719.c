xdr_krb5_flags(XDR *xdrs, krb5_flags *objp)
{
  /* This assumes that int32 and krb5_flags are the same size.
     This shouldn't be a problem, since we've got a unit test which
     checks for this. */
	if (!xdr_int32(xdrs, (int32_t *) objp)) {
		return (FALSE);
	}
	return (TRUE);
}
