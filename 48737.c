bool_t xdr_ui_4(XDR *xdrs, krb5_ui_4 *objp)
{
  /* Assumes that krb5_ui_4 and u_int32 are both four bytes long.
     This should not be a harmful assumption. */
  return xdr_u_int32(xdrs, (uint32_t *) objp);
}
