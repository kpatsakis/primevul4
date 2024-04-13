kdc_check_transited_list(kdc_realm_t *kdc_active_realm,
                         const krb5_data *trans,
                         const krb5_data *realm1,
                         const krb5_data *realm2)
{
    krb5_error_code             code;

    /* Check against the KDB module.  Treat this answer as authoritative if the
     * method is supported and doesn't explicitly pass control. */
    code = krb5_db_check_transited_realms(kdc_context, trans, realm1, realm2);
    if (code != KRB5_PLUGIN_OP_NOTSUPP && code != KRB5_PLUGIN_NO_HANDLE)
        return code;

    /* Check using krb5.conf [capaths] or hierarchical relationships. */
    return krb5_check_transited_list(kdc_context, trans, realm1, realm2);
}
