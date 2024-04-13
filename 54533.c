_kdc_is_anonymous(krb5_context context, krb5_principal principal)
{
    if ((principal->name.name_type != KRB5_NT_WELLKNOWN &&
	 principal->name.name_type != KRB5_NT_UNKNOWN) ||
	principal->name.name_string.len != 2 ||
	strcmp(principal->name.name_string.val[0], KRB5_WELLKNOWN_NAME) != 0 ||
	strcmp(principal->name.name_string.val[1], KRB5_ANON_NAME) != 0)
	return 0;
    return 1;
}
