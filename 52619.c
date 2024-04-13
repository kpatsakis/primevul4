update_ldap_mod_auth_ind(krb5_context context, krb5_db_entry *entry,
                         LDAPMod ***mods)
{
    int i = 0;
    krb5_error_code ret;
    char *auth_ind = NULL;
    char *strval[10] = {};
    char *ai, *ai_save = NULL;
    int sv_num = sizeof(strval) / sizeof(*strval);

    ret = krb5_dbe_get_string(context, entry, KRB5_KDB_SK_REQUIRE_AUTH,
                              &auth_ind);
    if (ret || auth_ind == NULL)
        goto cleanup;

    ai = strtok_r(auth_ind, " ", &ai_save);
    while (ai != NULL && i < sv_num) {
        strval[i++] = ai;
        ai = strtok_r(NULL, " ", &ai_save);
    }

    ret = krb5_add_str_mem_ldap_mod(mods, "krbPrincipalAuthInd",
                                    LDAP_MOD_REPLACE, strval);

cleanup:
    krb5_dbe_free_string(context, auth_ind);
    return ret;
}
