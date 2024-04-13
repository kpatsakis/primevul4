check_anon(kdc_realm_t *kdc_active_realm,
           krb5_principal client, krb5_principal server)
{
    /* If restrict_anon is set, reject requests from anonymous to principals
     * other than the local TGT. */
    if (kdc_active_realm->realm_restrict_anon &&
        krb5_principal_compare_any_realm(kdc_context, client,
                                         krb5_anonymous_principal()) &&
        !krb5_principal_compare(kdc_context, server, tgs_server))
        return -1;
    return 0;
}
