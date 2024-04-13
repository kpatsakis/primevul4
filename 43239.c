populate_policy(krb5_context context,
                LDAP *ld,
                LDAPMessage *ent,
                char *pol_name,
                osa_policy_ent_t pol_entry)
{
    int st = 0;

    pol_entry->name = strdup(pol_name);
    CHECK_NULL(pol_entry->name);
    pol_entry->version = 1;

    get_ui4(ld, ent, "krbmaxpwdlife", &pol_entry->pw_max_life);
    get_ui4(ld, ent, "krbminpwdlife", &pol_entry->pw_min_life);
    get_ui4(ld, ent, "krbpwdmindiffchars", &pol_entry->pw_min_classes);
    get_ui4(ld, ent, "krbpwdminlength", &pol_entry->pw_min_length);
    get_ui4(ld, ent, "krbpwdhistorylength", &pol_entry->pw_history_num);
    get_ui4(ld, ent, "krbpwdmaxfailure", &pol_entry->pw_max_fail);
    get_ui4(ld, ent, "krbpwdfailurecountinterval",
            &pol_entry->pw_failcnt_interval);
    get_ui4(ld, ent, "krbpwdlockoutduration", &pol_entry->pw_lockout_duration);
    get_ui4(ld, ent, "krbpwdattributes", &pol_entry->attributes);
    get_ui4(ld, ent, "krbpwdmaxlife", &pol_entry->max_life);
    get_ui4(ld, ent, "krbpwdmaxrenewablelife", &pol_entry->max_renewable_life);

    st = krb5_ldap_get_string(ld, ent, "krbpwdallowedkeysalts",
                              &(pol_entry->allowed_keysalts), NULL);
    if (st)
        goto cleanup;
    /*
     * We don't store the policy refcnt, because principals might be maintained
     * outside of kadmin.  Instead, we will check for principal references when
     * policies are deleted.
     */
    pol_entry->policy_refcnt = 0;

cleanup:
    return st;
}
