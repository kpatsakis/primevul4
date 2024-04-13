bool_t xdr_krb5_key_data_nocontents(XDR *xdrs, krb5_key_data *objp)
{
     /*
      * Note that this function intentionally DOES NOT tranfer key
      * length or contents!  xdr_krb5_key_data in adb_xdr.c does, but
      * that is only for use within the server-side library.
      */
     unsigned int tmp;

     if (xdrs->x_op == XDR_DECODE)
	  memset(objp, 0, sizeof(krb5_key_data));

     if (!xdr_krb5_int16(xdrs, &objp->key_data_ver)) {
	  return (FALSE);
     }
     if (!xdr_krb5_ui_2(xdrs, &objp->key_data_kvno)) {
	  return (FALSE);
     }
     if (!xdr_krb5_int16(xdrs, &objp->key_data_type[0])) {
	  return (FALSE);
     }
     if (objp->key_data_ver > 1) {
	  if (!xdr_krb5_int16(xdrs, &objp->key_data_type[1])) {
	       return (FALSE);
	  }
     }
     /*
      * kadm5_get_principal on the server side allocates and returns
      * key contents when asked.  Even though this function refuses to
      * transmit that data, it still has to *free* the data at the
      * appropriate time to avoid a memory leak.
      */
     if (xdrs->x_op == XDR_FREE) {
	  tmp = (unsigned int) objp->key_data_length[0];
	  if (!xdr_bytes(xdrs, (char **) &objp->key_data_contents[0],
			 &tmp, ~0))
	       return FALSE;

	  tmp = (unsigned int) objp->key_data_length[1];
	  if (!xdr_bytes(xdrs, (char **) &objp->key_data_contents[1],
			 &tmp, ~0))
	       return FALSE;
     }

     return (TRUE);
}
