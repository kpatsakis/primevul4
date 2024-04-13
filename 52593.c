is_local_principal(kdc_realm_t *kdc_active_realm, krb5_const_principal princ1)
{
    return krb5_realm_compare(kdc_context, princ1, tgs_server);
}
