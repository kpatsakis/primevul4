kadm5_setv4key_principal(void *server_handle,
                         krb5_principal principal,
                         krb5_keyblock *keyblock)
{
    krb5_db_entry               *kdb;
    osa_princ_ent_rec           adb;
    krb5_int32                  now;
    kadm5_policy_ent_rec        pol;
    krb5_keysalt                keysalt;
    int                         i, k, kvno, ret;
    krb5_boolean                have_pol = FALSE;
#if 0
    int                         last_pwd;
#endif
    kadm5_server_handle_t       handle = server_handle;
    krb5_key_data               tmp_key_data;
    krb5_keyblock               *act_mkey;

    memset( &tmp_key_data, 0, sizeof(tmp_key_data));

    CHECK_HANDLE(server_handle);

    krb5_clear_error_message(handle->context);

    if (principal == NULL || keyblock == NULL)
        return EINVAL;
    if (hist_princ && /* this will be NULL when initializing the databse */
        ((krb5_principal_compare(handle->context,
                                 principal, hist_princ)) == TRUE))
        return KADM5_PROTECT_PRINCIPAL;

    if (keyblock->enctype != ENCTYPE_DES_CBC_CRC)
        return KADM5_SETV4KEY_INVAL_ENCTYPE;

    if ((ret = kdb_get_entry(handle, principal, &kdb, &adb)))
        return(ret);

    for (kvno = 0, i=0; i<kdb->n_key_data; i++)
        if (kdb->key_data[i].key_data_kvno > kvno)
            kvno = kdb->key_data[i].key_data_kvno;

    if (kdb->key_data != NULL)
        cleanup_key_data(handle->context, kdb->n_key_data, kdb->key_data);

    kdb->key_data = (krb5_key_data*)krb5_db_alloc(handle->context, NULL, sizeof(krb5_key_data));
    if (kdb->key_data == NULL)
        return ENOMEM;
    memset(kdb->key_data, 0, sizeof(krb5_key_data));
    kdb->n_key_data = 1;
    keysalt.type = KRB5_KDB_SALTTYPE_V4;
    /* XXX data.magic? */
    keysalt.data.length = 0;
    keysalt.data.data = NULL;

    ret = kdb_get_active_mkey(handle, NULL, &act_mkey);
    if (ret)
        goto done;

    /* use tmp_key_data as temporary location and reallocate later */
    ret = krb5_dbe_encrypt_key_data(handle->context, act_mkey, keyblock,
                                    &keysalt, kvno + 1, &tmp_key_data);
    if (ret) {
        goto done;
    }

    for (k = 0; k < tmp_key_data.key_data_ver; k++) {
        kdb->key_data->key_data_type[k] = tmp_key_data.key_data_type[k];
        kdb->key_data->key_data_length[k] = tmp_key_data.key_data_length[k];
        if (tmp_key_data.key_data_contents[k]) {
            kdb->key_data->key_data_contents[k] = krb5_db_alloc(handle->context, NULL, tmp_key_data.key_data_length[k]);
            if (kdb->key_data->key_data_contents[k] == NULL) {
                cleanup_key_data(handle->context, kdb->n_key_data, kdb->key_data);
                kdb->key_data = NULL;
                kdb->n_key_data = 0;
                ret = ENOMEM;
                goto done;
            }
            memcpy (kdb->key_data->key_data_contents[k], tmp_key_data.key_data_contents[k], tmp_key_data.key_data_length[k]);

            memset (tmp_key_data.key_data_contents[k], 0, tmp_key_data.key_data_length[k]);
            free (tmp_key_data.key_data_contents[k]);
            tmp_key_data.key_data_contents[k] = NULL;
        }
    }



    kdb->attributes &= ~KRB5_KDB_REQUIRES_PWCHANGE;

    ret = krb5_timeofday(handle->context, &now);
    if (ret)
        goto done;

    if ((adb.aux_attributes & KADM5_POLICY)) {
        ret = get_policy(handle, adb.policy, &pol, &have_pol);
        if (ret)
            goto done;
    }
    if (have_pol) {
#if 0
        /*
         * The spec says this check is overridden if the caller has
         * modify privilege.  The admin server therefore makes this
         * check itself (in chpass_principal_wrapper, misc.c).  A
         * local caller implicitly has all authorization bits.
         */
        if (ret = krb5_dbe_lookup_last_pwd_change(handle->context,
                                                  kdb, &last_pwd))
            goto done;
        if((now - last_pwd) < pol.pw_min_life &&
           !(kdb->attributes & KRB5_KDB_REQUIRES_PWCHANGE)) {
            ret = KADM5_PASS_TOOSOON;
            goto done;
        }
#endif

        if (pol.pw_max_life)
            kdb->pw_expiration = now + pol.pw_max_life;
        else
            kdb->pw_expiration = 0;
    } else {
        kdb->pw_expiration = 0;
    }

    ret = krb5_dbe_update_last_pwd_change(handle->context, kdb, now);
    if (ret)
        goto done;

    /* unlock principal on this KDC */
    kdb->fail_auth_count = 0;

    if ((ret = kdb_put_entry(handle, kdb, &adb)))
        goto done;

    ret = KADM5_OK;
done:
    for (i = 0; i < tmp_key_data.key_data_ver; i++) {
        if (tmp_key_data.key_data_contents[i]) {
            memset (tmp_key_data.key_data_contents[i], 0, tmp_key_data.key_data_length[i]);
            free (tmp_key_data.key_data_contents[i]);
        }
    }

    kdb_free_entry(handle, kdb, &adb);
    if (have_pol)
        kadm5_free_policy_ent(handle->lhandle, &pol);

    return ret;
}
