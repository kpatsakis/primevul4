select_session_keytype(kdc_realm_t *kdc_active_realm, krb5_db_entry *server,
                       int nktypes, krb5_enctype *ktype)
{
    int         i;

    for (i = 0; i < nktypes; i++) {
        if (!krb5_c_valid_enctype(ktype[i]))
            continue;

        if (!krb5_is_permitted_enctype(kdc_context, ktype[i]))
            continue;

        if (dbentry_supports_enctype(kdc_active_realm, server, ktype[i]))
            return ktype[i];
    }

    return 0;
}
