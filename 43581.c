krb5_read_tkt_policy(krb5_context context, krb5_ldap_context *ldap_context,
                     krb5_db_entry *entries, char *policy)
{
    krb5_error_code             st=0;
    int                         mask=0, omask=0;
    int                         tkt_mask=(KDB_MAX_LIFE_ATTR | KDB_MAX_RLIFE_ATTR | KDB_TKT_FLAGS_ATTR);
    krb5_ldap_policy_params     *tktpoldnparam=NULL;

    if ((st=krb5_get_attributes_mask(context, entries, &mask)) != 0)
        goto cleanup;

    if ((mask & tkt_mask) == tkt_mask)
        goto cleanup;

    if (policy != NULL) {
        st = krb5_ldap_read_policy(context, policy, &tktpoldnparam, &omask);
        if (st && st != KRB5_KDB_NOENTRY) {
            prepend_err_str(context, _("Error reading ticket policy. "), st,
                            st);
            goto cleanup;
        }

        st = 0; /* reset the return status */
    }

    if ((mask & KDB_MAX_LIFE_ATTR) == 0) {
        if ((omask & KDB_MAX_LIFE_ATTR) ==  KDB_MAX_LIFE_ATTR)
            entries->max_life = tktpoldnparam->maxtktlife;
        else if (ldap_context->lrparams->max_life)
            entries->max_life = ldap_context->lrparams->max_life;
    }

    if ((mask & KDB_MAX_RLIFE_ATTR) == 0) {
        if ((omask & KDB_MAX_RLIFE_ATTR) == KDB_MAX_RLIFE_ATTR)
            entries->max_renewable_life = tktpoldnparam->maxrenewlife;
        else if (ldap_context->lrparams->max_renewable_life)
            entries->max_renewable_life = ldap_context->lrparams->max_renewable_life;
    }

    if ((mask & KDB_TKT_FLAGS_ATTR) == 0) {
        if ((omask & KDB_TKT_FLAGS_ATTR) == KDB_TKT_FLAGS_ATTR)
            entries->attributes = tktpoldnparam->tktflags;
        else if (ldap_context->lrparams->tktflags)
            entries->attributes |= ldap_context->lrparams->tktflags;
    }
    krb5_ldap_free_policy(context, tktpoldnparam);

cleanup:
    return st;
}
