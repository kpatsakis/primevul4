xdr_krb5_enctype(XDR *xdrs, krb5_enctype *objp)
{
   /*
    * This used to be xdr_krb5_keytype, but keytypes and enctypes have
    * been merged into only enctypes.  However, randkey_principal
    * already ensures that only a key of ENCTYPE_DES_CBC_CRC will be
    * returned to v1 clients, and ENCTYPE_DES_CBC_CRC has the same
    * value as KEYTYPE_DES used too, which is what all v1 clients
    * expect.  Therefore, IMHO, just encoding whatever enctype we get
    * is safe.
    */

   if (!xdr_int32(xdrs, (int32_t *) objp))
	return (FALSE);
   return (TRUE);
}
