krb5_ldap_get_principal(krb5_context context, krb5_const_principal searchfor,
                        unsigned int flags, krb5_db_entry **entry_ptr)
{
    char                        *user=NULL, *filter=NULL, *filtuser=NULL;
    unsigned int                tree=0, ntrees=1, princlen=0;
    krb5_error_code             tempst=0, st=0;
    char                        **values=NULL, **subtree=NULL, *cname=NULL;
    LDAP                        *ld=NULL;
    LDAPMessage                 *result=NULL, *ent=NULL;
    krb5_ldap_context           *ldap_context=NULL;
    kdb5_dal_handle             *dal_handle=NULL;
    krb5_ldap_server_handle     *ldap_server_handle=NULL;
    krb5_principal              cprinc=NULL;
    krb5_boolean                found=FALSE;
    krb5_db_entry               *entry = NULL;

    *entry_ptr = NULL;

    /* Clear the global error string */
    krb5_clear_error_message(context);

    if (searchfor == NULL)
        return EINVAL;

    dal_handle = context->dal_handle;
    ldap_context = (krb5_ldap_context *) dal_handle->db_context;

    CHECK_LDAP_HANDLE(ldap_context);

    if (is_principal_in_realm(ldap_context, searchfor) != 0) {
        st = KRB5_KDB_NOENTRY;
        krb5_set_error_message(context, st,
                               _("Principal does not belong to realm"));
        goto cleanup;
    }

    if ((st=krb5_unparse_name(context, searchfor, &user)) != 0)
        goto cleanup;

    if ((st=krb5_ldap_unparse_principal_name(user)) != 0)
        goto cleanup;

    filtuser = ldap_filter_correct(user);
    if (filtuser == NULL) {
        st = ENOMEM;
        goto cleanup;
    }

    princlen = strlen(FILTER) + strlen(filtuser) + 2 + 1;  /* 2 for closing brackets */
    if ((filter = malloc(princlen)) == NULL) {
        st = ENOMEM;
        goto cleanup;
    }
    snprintf(filter, princlen, FILTER"%s))", filtuser);

    if ((st = krb5_get_subtree_info(ldap_context, &subtree, &ntrees)) != 0)
        goto cleanup;

    GET_HANDLE();
    for (tree=0; tree < ntrees && !found; ++tree) {

        LDAP_SEARCH(subtree[tree], ldap_context->lrparams->search_scope, filter, principal_attributes);
        for (ent=ldap_first_entry(ld, result); ent != NULL && !found; ent=ldap_next_entry(ld, ent)) {

            /* get the associated directory user information */
            if ((values=ldap_get_values(ld, ent, "krbprincipalname")) != NULL) {
                int i;

                /* a wild-card in a principal name can return a list of kerberos principals.
                 * Make sure that the correct principal is returned.
                 * NOTE: a principalname k* in ldap server will return all the principals starting with a k
                 */
                for (i=0; values[i] != NULL; ++i) {
                    if (strcmp(values[i], user) == 0) {
                        found = TRUE;
                        break;
                    }
                }
                ldap_value_free(values);

                if (!found) /* no matching principal found */
                    continue;
            }

            if ((values=ldap_get_values(ld, ent, "krbcanonicalname")) != NULL) {
                if (values[0] && strcmp(values[0], user) != 0) {
                    /* We matched an alias, not the canonical name. */
                    if (flags & KRB5_KDB_FLAG_ALIAS_OK) {
                        st = krb5_ldap_parse_principal_name(values[0], &cname);
                        if (st != 0)
                            goto cleanup;
                        st = krb5_parse_name(context, cname, &cprinc);
                        if (st != 0)
                            goto cleanup;
                    } else /* No canonicalization, so don't return aliases. */
                        found = FALSE;
                }
                ldap_value_free(values);
                if (!found)
                    continue;
            }

            entry = k5alloc(sizeof(*entry), &st);
            if (entry == NULL)
                goto cleanup;
            if ((st = populate_krb5_db_entry(context, ldap_context, ld, ent,
                                             cprinc ? cprinc : searchfor,
                                             entry)) != 0)
                goto cleanup;
        }
        ldap_msgfree(result);
        result = NULL;
    } /* for (tree=0 ... */

    if (found) {
        *entry_ptr = entry;
        entry = NULL;
    } else
        st = KRB5_KDB_NOENTRY;

cleanup:
    ldap_msgfree(result);
    krb5_ldap_free_principal(context, entry);

    if (filter)
        free (filter);

    if (subtree) {
        for (; ntrees; --ntrees)
            if (subtree[ntrees-1])
                free (subtree[ntrees-1]);
        free (subtree);
    }

    if (ldap_server_handle)
        krb5_ldap_put_handle_to_pool(ldap_context, ldap_server_handle);

    if (user)
        free(user);

    if (filtuser)
        free(filtuser);

    if (cname)
        free(cname);

    if (cprinc)
        krb5_free_principal(context, cprinc);

    return st;
}
