kadm5_setkey_principal_3(void *server_handle,
                         krb5_principal principal,
                         krb5_boolean keepold,
                         int n_ks_tuple, krb5_key_salt_tuple *ks_tuple,
                         krb5_keyblock *keyblocks,
                         int n_keys)
{
    krb5_db_entry               *kdb;
    osa_princ_ent_rec           adb;
    krb5_int32                  now;
    kadm5_policy_ent_rec        pol;
    krb5_key_data               *old_key_data;
    int                         n_old_keys;
    int                         i, j, k, kvno, ret;
    krb5_boolean                have_pol = FALSE;
#if 0
    int                         last_pwd;
#endif
    kadm5_server_handle_t       handle = server_handle;
    krb5_boolean                similar;
    krb5_keysalt                keysalt;
    krb5_key_data         tmp_key_data;
    krb5_key_data        *tptr;
    krb5_keyblock               *act_mkey;
    krb5_key_salt_tuple         *ks_from_keys = NULL;

    CHECK_HANDLE(server_handle);

    krb5_clear_error_message(handle->context);

    if (principal == NULL || keyblocks == NULL)
        return EINVAL;
    if (hist_princ && /* this will be NULL when initializing the databse */
        ((krb5_principal_compare(handle->context,
                                 principal, hist_princ)) == TRUE))
        return KADM5_PROTECT_PRINCIPAL;

    if ((ret = kdb_get_entry(handle, principal, &kdb, &adb)))
        return(ret);

    if (!n_ks_tuple) {
        /* Apply policy to the key/salt types implied by the given keys */
        ret = make_ks_from_keys(handle->context, n_keys, keyblocks,
                                &ks_from_keys);
        if (ret)
            goto done;
        ret = apply_keysalt_policy(handle, adb.policy, n_keys, ks_from_keys,
                                   NULL, NULL);
        free(ks_from_keys);
    } else {
        /*
         * Apply policy to the given ks_tuples.  Note that further below
         * we enforce keyblocks[i].enctype == ks_tuple[i].ks_enctype for
         * all i from 0 to n_keys, and that n_ks_tuple == n_keys if ks
         * tuples are given.
         */
        ret = apply_keysalt_policy(handle, adb.policy, n_ks_tuple, ks_tuple,
                                   NULL, NULL);
    }
    if (ret)
        goto done;

    for (i = 0; i < n_keys; i++) {
        for (j = i+1; j < n_keys; j++) {
            if ((ret = krb5_c_enctype_compare(handle->context,
                                              keyblocks[i].enctype,
                                              keyblocks[j].enctype,
                                              &similar)))
                return(ret);
            if (similar) {
                if (n_ks_tuple) {
                    if (ks_tuple[i].ks_salttype == ks_tuple[j].ks_salttype)
                        return KADM5_SETKEY_DUP_ENCTYPES;
                } else
                    return KADM5_SETKEY_DUP_ENCTYPES;
            }
        }
    }

    if (n_ks_tuple && n_ks_tuple != n_keys)
        return KADM5_SETKEY3_ETYPE_MISMATCH;

    for (kvno = 0, i=0; i<kdb->n_key_data; i++)
        if (kdb->key_data[i].key_data_kvno > kvno)
            kvno = kdb->key_data[i].key_data_kvno;

    if (keepold) {
        old_key_data = kdb->key_data;
        n_old_keys = kdb->n_key_data;
    } else {
        if (kdb->key_data != NULL)
            cleanup_key_data(handle->context, kdb->n_key_data, kdb->key_data);
        n_old_keys = 0;
        old_key_data = NULL;
    }

    /* Allocate one extra key_data to avoid allocating 0 bytes. */
    kdb->key_data = krb5_db_alloc(handle->context, NULL,
                                  (n_keys + n_old_keys + 1) *
                                  sizeof(krb5_key_data));
    if (kdb->key_data == NULL) {
        ret = ENOMEM;
        goto done;
    }

    memset(kdb->key_data, 0, (n_keys+n_old_keys)*sizeof(krb5_key_data));
    kdb->n_key_data = 0;

    for (i = 0; i < n_keys; i++) {
        if (n_ks_tuple) {
            keysalt.type = ks_tuple[i].ks_salttype;
            keysalt.data.length = 0;
            keysalt.data.data = NULL;
            if (ks_tuple[i].ks_enctype != keyblocks[i].enctype) {
                ret = KADM5_SETKEY3_ETYPE_MISMATCH;
                goto done;
            }
        }
        memset (&tmp_key_data, 0, sizeof(tmp_key_data));

        ret = kdb_get_active_mkey(handle, NULL, &act_mkey);
        if (ret)
            goto done;

        ret = krb5_dbe_encrypt_key_data(handle->context, act_mkey,
                                        &keyblocks[i],
                                        n_ks_tuple ? &keysalt : NULL, kvno + 1,
                                        &tmp_key_data);
        if (ret)
            goto done;

        tptr = &kdb->key_data[i];
        tptr->key_data_ver = tmp_key_data.key_data_ver;
        tptr->key_data_kvno = tmp_key_data.key_data_kvno;
        for (k = 0; k < tmp_key_data.key_data_ver; k++) {
            tptr->key_data_type[k] = tmp_key_data.key_data_type[k];
            tptr->key_data_length[k] = tmp_key_data.key_data_length[k];
            if (tmp_key_data.key_data_contents[k]) {
                tptr->key_data_contents[k] = krb5_db_alloc(handle->context, NULL, tmp_key_data.key_data_length[k]);
                if (tptr->key_data_contents[k] == NULL) {
                    int i1;
                    for (i1 = k; i1 < tmp_key_data.key_data_ver; i1++) {
                        if (tmp_key_data.key_data_contents[i1]) {
                            memset (tmp_key_data.key_data_contents[i1], 0, tmp_key_data.key_data_length[i1]);
                            free (tmp_key_data.key_data_contents[i1]);
                        }
                    }

                    ret =  ENOMEM;
                    goto done;
                }
                memcpy (tptr->key_data_contents[k], tmp_key_data.key_data_contents[k], tmp_key_data.key_data_length[k]);

                memset (tmp_key_data.key_data_contents[k], 0, tmp_key_data.key_data_length[k]);
                free (tmp_key_data.key_data_contents[k]);
                tmp_key_data.key_data_contents[k] = NULL;
            }
        }
        kdb->n_key_data++;
    }

    /* copy old key data if necessary */
    for (i = 0; i < n_old_keys; i++) {
        kdb->key_data[i+n_keys] = old_key_data[i];
        memset(&old_key_data[i], 0, sizeof (krb5_key_data));
        kdb->n_key_data++;
    }

    if (old_key_data)
        krb5_db_free(handle->context, old_key_data);

    /* assert(kdb->n_key_data == n_keys + n_old_keys) */
    kdb->attributes &= ~KRB5_KDB_REQUIRES_PWCHANGE;

    if ((ret = krb5_timeofday(handle->context, &now)))
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

    if ((ret = krb5_dbe_update_last_pwd_change(handle->context, kdb, now)))
        goto done;

    /* unlock principal on this KDC */
    kdb->fail_auth_count = 0;

    if ((ret = kdb_put_entry(handle, kdb, &adb)))
        goto done;

    ret = KADM5_OK;
done:
    kdb_free_entry(handle, kdb, &adb);
    if (have_pol)
        kadm5_free_policy_ent(handle->lhandle, &pol);

    return ret;
}
