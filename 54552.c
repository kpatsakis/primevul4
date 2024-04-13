older_enctype(krb5_enctype enctype)
{
    switch (enctype) {
    case ETYPE_DES_CBC_CRC:
    case ETYPE_DES_CBC_MD4:
    case ETYPE_DES_CBC_MD5:
    case ETYPE_DES3_CBC_SHA1:
    case ETYPE_ARCFOUR_HMAC_MD5:
    case ETYPE_ARCFOUR_HMAC_MD5_56:
    /*
     * The following three is "old" windows enctypes and is needed for
     * windows 2000 hosts.
     */
    case ETYPE_ARCFOUR_MD4:
    case ETYPE_ARCFOUR_HMAC_OLD:
    case ETYPE_ARCFOUR_HMAC_OLD_EXP:
	return 1;
    default:
	return 0;
    }
}
