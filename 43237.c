krb5_ldap_get_password_policy(krb5_context context, char *name,
                              osa_policy_ent_t *policy)
{
    krb5_error_code             st = 0;
    char                        *policy_dn = NULL;

    /* Clear the global error string */
    krb5_clear_error_message(context);

    /* validate the input parameters */
    if (name == NULL) {
        st = EINVAL;
        goto cleanup;
    }

    st = krb5_ldap_name_to_policydn(context, name, &policy_dn);
    if (st != 0)
        goto cleanup;

    st = krb5_ldap_get_password_policy_from_dn(context, name, policy_dn,
                                               policy);

cleanup:
    free(policy_dn);
    return st;
}
