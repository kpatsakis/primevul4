xdr_krb5_deltat(XDR *xdrs, krb5_deltat *objp)
{
  /* This assumes that int32 and krb5_deltat are the same size.
     This shouldn't be a problem, since we've got a unit test which
     checks for this. */
	if (!xdr_int32(xdrs, (int32_t *) objp)) {
		return (FALSE);
	}
	return (TRUE);
}
