is_default_salt_p(const krb5_salt *default_salt, const Key *key)
{
    if (key->salt == NULL)
	return TRUE;
    if (default_salt->salttype != key->salt->type)
	return FALSE;
    if (krb5_data_cmp(&default_salt->saltvalue, &key->salt->salt))
	return FALSE;
    return TRUE;
}
