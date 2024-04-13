check_indicators(krb5_context context, krb5_db_entry *server,
                 krb5_data *const *indicators)
{
    krb5_error_code ret;
    char *str = NULL, *copy = NULL, *save, *ind;

    ret = krb5_dbe_get_string(context, server, KRB5_KDB_SK_REQUIRE_AUTH, &str);
    if (ret || str == NULL)
        goto cleanup;
    copy = strdup(str);
    if (copy == NULL) {
        ret = ENOMEM;
        goto cleanup;
    }

    /* Look for any of the space-separated strings in indicators. */
    ind = strtok_r(copy, " ", &save);
    while (ind != NULL) {
        if (authind_contains(indicators, ind))
            goto cleanup;
        ind = strtok_r(NULL, " ", &save);
    }

    ret = KRB5KDC_ERR_POLICY;
    k5_setmsg(context, ret,
              _("Required auth indicators not present in ticket: %s"), str);

cleanup:
    krb5_dbe_free_string(context, str);
    free(copy);
    return ret;
}
