_kdc_check_addresses(krb5_context context,
		     krb5_kdc_configuration *config,
		     HostAddresses *addresses, const struct sockaddr *from)
{
    krb5_error_code ret;
    krb5_address addr;
    krb5_boolean result;
    krb5_boolean only_netbios = TRUE;
    size_t i;

    if(config->check_ticket_addresses == 0)
	return TRUE;

    if(addresses == NULL)
	return config->allow_null_ticket_addresses;

    for (i = 0; i < addresses->len; ++i) {
	if (addresses->val[i].addr_type != KRB5_ADDRESS_NETBIOS) {
	    only_netbios = FALSE;
	}
    }

    /* Windows sends it's netbios name, which I can only assume is
     * used for the 'allowed workstations' check.  This is painful,
     * but we still want to check IP addresses if they happen to be
     * present.
     */

    if(only_netbios)
	return config->allow_null_ticket_addresses;

    ret = krb5_sockaddr2address (context, from, &addr);
    if(ret)
	return FALSE;

    result = krb5_address_search(context, &addr, addresses);
    krb5_free_address (context, &addr);
    return result;
}
