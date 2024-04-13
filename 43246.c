kadm5_get_principal(void *server_handle, krb5_principal principal,
                    kadm5_principal_ent_t entry,
                    long in_mask)
{
    krb5_db_entry               *kdb;
    osa_princ_ent_rec           adb;
    krb5_error_code             ret = 0;
    long                        mask;
    int i;
    kadm5_server_handle_t handle = server_handle;

    CHECK_HANDLE(server_handle);

    krb5_clear_error_message(handle->context);

    /*
     * In version 1, all the defined fields are always returned.
     * entry is a pointer to a kadm5_principal_ent_t_v1 that should be
     * filled with allocated memory.
     */
    mask = in_mask;

    memset(entry, 0, sizeof(*entry));

    if (principal == NULL)
        return EINVAL;

    if ((ret = kdb_get_entry(handle, principal, &kdb, &adb)))
        return ret;

    if ((mask & KADM5_POLICY) &&
        adb.policy && (adb.aux_attributes & KADM5_POLICY)) {
        if ((entry->policy = strdup(adb.policy)) == NULL) {
            ret = ENOMEM;
            goto done;
        }
    }

    if (mask & KADM5_AUX_ATTRIBUTES)
        entry->aux_attributes = adb.aux_attributes;

    if ((mask & KADM5_PRINCIPAL) &&
        (ret = krb5_copy_principal(handle->context, kdb->princ,
                                   &entry->principal))) {
        goto done;
    }

    if (mask & KADM5_PRINC_EXPIRE_TIME)
        entry->princ_expire_time = kdb->expiration;

    if ((mask & KADM5_LAST_PWD_CHANGE) &&
        (ret = krb5_dbe_lookup_last_pwd_change(handle->context, kdb,
                                               &(entry->last_pwd_change)))) {
        goto done;
    }

    if (mask & KADM5_PW_EXPIRATION)
        entry->pw_expiration = kdb->pw_expiration;
    if (mask & KADM5_MAX_LIFE)
        entry->max_life = kdb->max_life;

    /* this is a little non-sensical because the function returns two */
    /* values that must be checked separately against the mask */
    if ((mask & KADM5_MOD_NAME) || (mask & KADM5_MOD_TIME)) {
        ret = krb5_dbe_lookup_mod_princ_data(handle->context, kdb,
                                             &(entry->mod_date),
                                             &(entry->mod_name));
        if (ret) {
            goto done;
        }

        if (! (mask & KADM5_MOD_TIME))
            entry->mod_date = 0;
        if (! (mask & KADM5_MOD_NAME)) {
            krb5_free_principal(handle->context, entry->mod_name);
            entry->mod_name = NULL;
        }
    }

    if (mask & KADM5_ATTRIBUTES)
        entry->attributes = kdb->attributes;

    if (mask & KADM5_KVNO)
        for (entry->kvno = 0, i=0; i<kdb->n_key_data; i++)
            if ((krb5_kvno) kdb->key_data[i].key_data_kvno > entry->kvno)
                entry->kvno = kdb->key_data[i].key_data_kvno;

    if (mask & KADM5_MKVNO) {
        ret = krb5_dbe_get_mkvno(handle->context, kdb, &entry->mkvno);
        if (ret)
            goto done;
    }

    if (mask & KADM5_MAX_RLIFE)
        entry->max_renewable_life = kdb->max_renewable_life;
    if (mask & KADM5_LAST_SUCCESS)
        entry->last_success = kdb->last_success;
    if (mask & KADM5_LAST_FAILED)
        entry->last_failed = kdb->last_failed;
    if (mask & KADM5_FAIL_AUTH_COUNT)
        entry->fail_auth_count = kdb->fail_auth_count;
    if (mask & KADM5_TL_DATA) {
        krb5_tl_data *tl, *tl2;

        entry->tl_data = NULL;

        tl = kdb->tl_data;
        while (tl) {
            if (tl->tl_data_type > 255) {
                if ((tl2 = dup_tl_data(tl)) == NULL) {
                    ret = ENOMEM;
                    goto done;
                }
                tl2->tl_data_next = entry->tl_data;
                entry->tl_data = tl2;
                entry->n_tl_data++;
            }

            tl = tl->tl_data_next;
        }
    }
    if (mask & KADM5_KEY_DATA) {
        entry->n_key_data = kdb->n_key_data;
        if(entry->n_key_data) {
            entry->key_data = k5calloc(entry->n_key_data,
                                       sizeof(krb5_key_data), &ret);
            if (entry->key_data == NULL)
                goto done;
        } else
            entry->key_data = NULL;

        for (i = 0; i < entry->n_key_data; i++)
            ret = krb5_copy_key_data_contents(handle->context,
                                              &kdb->key_data[i],
                                              &entry->key_data[i]);
        if (ret)
            goto done;
    }

    ret = KADM5_OK;

done:
    if (ret && entry->principal) {
        krb5_free_principal(handle->context, entry->principal);
        entry->principal = NULL;
    }
    kdb_free_entry(handle, kdb, &adb);

    return ret;
}
