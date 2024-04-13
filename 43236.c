add_policy_mods(krb5_context context, LDAPMod ***mods, osa_policy_ent_t policy,
                int op)
{
    krb5_error_code st;
    char *strval[2] = { NULL };

    st = krb5_add_int_mem_ldap_mod(mods, "krbmaxpwdlife", op,
                                   (int)policy->pw_max_life);
    if (st)
        return st;

    st = krb5_add_int_mem_ldap_mod(mods, "krbminpwdlife", op,
                                   (int)policy->pw_min_life);
    if (st)
        return st;

    st = krb5_add_int_mem_ldap_mod(mods, "krbpwdmindiffchars", op,
                                   (int)policy->pw_min_classes);
    if (st)
        return st;

    st = krb5_add_int_mem_ldap_mod(mods, "krbpwdminlength", op,
                                   (int)policy->pw_min_length);
    if (st)
        return st;

    st = krb5_add_int_mem_ldap_mod(mods, "krbpwdhistorylength", op,
                                   (int)policy->pw_history_num);
    if (st)
        return st;

    st = krb5_add_int_mem_ldap_mod(mods, "krbpwdmaxfailure", op,
                                   (int)policy->pw_max_fail);
    if (st)
        return st;

    st = krb5_add_int_mem_ldap_mod(mods, "krbpwdfailurecountinterval", op,
                                   (int)policy->pw_failcnt_interval);
    if (st)
        return st;

    st = krb5_add_int_mem_ldap_mod(mods, "krbpwdlockoutduration", op,
                                   (int)policy->pw_lockout_duration);
    if (st)
        return st;

    st = krb5_add_int_mem_ldap_mod(mods, "krbpwdattributes", op,
                                   (int)policy->attributes);
    if (st)
        return st;

    st = krb5_add_int_mem_ldap_mod(mods, "krbpwdmaxlife", op,
                                   (int)policy->max_life);
    if (st)
        return st;

    st = krb5_add_int_mem_ldap_mod(mods, "krbpwdmaxrenewablelife", op,
                                   (int)policy->max_renewable_life);
    if (st)
        return st;

    if (policy->allowed_keysalts != NULL) {
        strval[0] = policy->allowed_keysalts;
        st = krb5_add_str_mem_ldap_mod(mods, "krbpwdallowedkeysalts",
                                       op, strval);
        if (st)
            return st;
    }

    /*
     * Each policy tl-data type we add should be explicitly marshalled here.
     * Unlike principals, we do not marshal unrecognized policy tl-data.
     */

    return 0;
}
