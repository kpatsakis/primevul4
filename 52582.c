check_allowed_to_delegate_to(krb5_context context, krb5_const_principal client,
                             const krb5_db_entry *server,
                             krb5_const_principal proxy)
{
    /* Can't get a TGT (otherwise it would be unconstrained delegation) */
    if (krb5_is_tgs_principal(proxy))
        return KRB5KDC_ERR_POLICY;

    /* Must be in same realm */
    if (!krb5_realm_compare(context, server->princ, proxy))
        return KRB5KDC_ERR_POLICY;

    return krb5_db_check_allowed_to_delegate(context, client, server, proxy);
}
