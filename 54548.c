make_etype_info2_entry(ETYPE_INFO2_ENTRY *ent, Key *key)
{
    krb5_error_code ret;

    ent->etype = key->key.keytype;
    if(key->salt) {
	ALLOC(ent->salt);
	if (ent->salt == NULL)
	    return ENOMEM;
	*ent->salt = malloc(key->salt->salt.length + 1);
	if (*ent->salt == NULL) {
	    free(ent->salt);
	    ent->salt = NULL;
	    return ENOMEM;
	}
	memcpy(*ent->salt, key->salt->salt.data, key->salt->salt.length);
	(*ent->salt)[key->salt->salt.length] = '\0';
    } else
	ent->salt = NULL;

    ent->s2kparams = NULL;

    switch (key->key.keytype) {
    case ETYPE_AES128_CTS_HMAC_SHA1_96:
    case ETYPE_AES256_CTS_HMAC_SHA1_96:
	ret = make_s2kparams(_krb5_AES_SHA1_string_to_default_iterator,
			     4, &ent->s2kparams);
	break;
    case KRB5_ENCTYPE_AES128_CTS_HMAC_SHA256_128:
    case KRB5_ENCTYPE_AES256_CTS_HMAC_SHA384_192:
	ret = make_s2kparams(_krb5_AES_SHA2_string_to_default_iterator,
			     4, &ent->s2kparams);
	break;
    case ETYPE_DES_CBC_CRC:
    case ETYPE_DES_CBC_MD4:
    case ETYPE_DES_CBC_MD5:
	/* Check if this was a AFS3 salted key */
	if(key->salt && key->salt->type == hdb_afs3_salt)
	    ret = make_s2kparams(1, 1, &ent->s2kparams);
	else
	    ret = 0;
	break;
    default:
	ret = 0;
	break;
    }
    return ret;
}
