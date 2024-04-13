_kdc_is_weak_exception(krb5_principal principal, krb5_enctype etype)
{
    if (principal->name.name_string.len > 0 &&
	strcmp(principal->name.name_string.val[0], "afs") == 0 &&
	(etype == (krb5_enctype)ETYPE_DES_CBC_CRC
	 || etype == (krb5_enctype)ETYPE_DES_CBC_MD4
	 || etype == (krb5_enctype)ETYPE_DES_CBC_MD5))
	return TRUE;
    return FALSE;
}
