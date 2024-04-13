kadm5_chpass_principal_3(void *server_handle,
                         krb5_principal principal, krb5_boolean keepold,
                         int n_ks_tuple, krb5_key_salt_tuple *ks_tuple,
                         char *password)
{
    krb5_int32                  now;
    kadm5_policy_ent_rec        pol;
    osa_princ_ent_rec           adb;
    krb5_db_entry               *kdb;
    int                         ret, ret2, last_pwd, hist_added;
    krb5_boolean                have_pol = FALSE;
    kadm5_server_handle_t       handle = server_handle;
    osa_pw_hist_ent             hist;
    krb5_keyblock               *act_mkey, *hist_keyblocks = NULL;
    krb5_kvno                   act_kvno, hist_kvno;
    int                         new_n_ks_tuple = 0;
    krb5_key_salt_tuple         *new_ks_tuple = NULL;

    CHECK_HANDLE(server_handle);

    krb5_clear_error_message(handle->context);

    hist_added = 0;
    memset(&hist, 0, sizeof(hist));

    if (principal == NULL || password == NULL)
        return EINVAL;
    if ((krb5_principal_compare(handle->context,
                                principal, hist_princ)) == TRUE)
        return KADM5_PROTECT_PRINCIPAL;

    if ((ret = kdb_get_entry(handle, principal, &kdb, &adb)))
        return(ret);

    ret = apply_keysalt_policy(handle, adb.policy, n_ks_tuple, ks_tuple,
                               &new_n_ks_tuple, &new_ks_tuple);
    if (ret)
        goto done;

    if ((adb.aux_attributes & KADM5_POLICY)) {
        ret = get_policy(handle, adb.policy, &pol, &have_pol);
        if (ret)
            goto done;
    }
    if (have_pol) {
        /* Create a password history entry before we change kdb's key_data. */
        ret = kdb_get_hist_key(handle, &hist_keyblocks, &hist_kvno);
        if (ret)
            goto done;
        ret = create_history_entry(handle->context, &hist_keyblocks[0],
                                   kdb->n_key_data, kdb->key_data, &hist);
        if (ret)
            goto done;
    }

    if ((ret = passwd_check(handle, password, have_pol ? &pol : NULL,
                            principal)))
        goto done;

    ret = kdb_get_active_mkey(handle, &act_kvno, &act_mkey);
    if (ret)
        goto done;

    ret = krb5_dbe_cpw(handle->context, act_mkey, new_ks_tuple, new_n_ks_tuple,
                       password, 0 /* increment kvno */,
                       keepold, kdb);
    if (ret)
        goto done;

    ret = krb5_dbe_update_mkvno(handle->context, kdb, act_kvno);
    if (ret)
        goto done;

    kdb->attributes &= ~KRB5_KDB_REQUIRES_PWCHANGE;

    ret = krb5_timeofday(handle->context, &now);
    if (ret)
        goto done;

    if ((adb.aux_attributes & KADM5_POLICY)) {
        /* the policy was loaded before */

        ret = krb5_dbe_lookup_last_pwd_change(handle->context, kdb, &last_pwd);
        if (ret)
            goto done;

#if 0
        /*
         * The spec says this check is overridden if the caller has
         * modify privilege.  The admin server therefore makes this
         * check itself (in chpass_principal_wrapper, misc.c). A
         * local caller implicitly has all authorization bits.
         */
        if ((now - last_pwd) < pol.pw_min_life &&
            !(kdb->attributes & KRB5_KDB_REQUIRES_PWCHANGE)) {
            ret = KADM5_PASS_TOOSOON;
            goto done;
        }
#endif

        ret = check_pw_reuse(handle->context, hist_keyblocks,
                             kdb->n_key_data, kdb->key_data,
                             1, &hist);
        if (ret)
            goto done;

        if (pol.pw_history_num > 1) {
            /* If hist_kvno has changed since the last password change, we
             * can't check the history. */
            if (adb.admin_history_kvno == hist_kvno) {
                ret = check_pw_reuse(handle->context, hist_keyblocks,
                                     kdb->n_key_data, kdb->key_data,
                                     adb.old_key_len, adb.old_keys);
                if (ret)
                    goto done;
            }

            ret = add_to_history(handle->context, hist_kvno, &adb, &pol,
                                 &hist);
            if (ret)
                goto done;
            hist_added = 1;
        }

        if (pol.pw_max_life)
            kdb->pw_expiration = now + pol.pw_max_life;
        else
            kdb->pw_expiration = 0;
    } else {
        kdb->pw_expiration = 0;
    }

#ifdef USE_PASSWORD_SERVER
    if (kadm5_use_password_server () &&
        (krb5_princ_size (handle->context, principal) == 1)) {
        krb5_data *princ = krb5_princ_component (handle->context, principal, 0);
        const char *path = "/usr/sbin/mkpassdb";
        char *argv[] = { "mkpassdb", "-setpassword", NULL, NULL };
        char *pstring = NULL;

        if (!ret) {
            pstring = malloc ((princ->length + 1) * sizeof (char));
            if (pstring == NULL) { ret = ENOMEM; }
        }

        if (!ret) {
            memcpy (pstring, princ->data, princ->length);
            pstring [princ->length] = '\0';
            argv[2] = pstring;

            ret = kadm5_launch_task (handle->context, path, argv, password);
        }

        if (pstring != NULL)
            free (pstring);

        if (ret)
            goto done;
    }
#endif

    ret = krb5_dbe_update_last_pwd_change(handle->context, kdb, now);
    if (ret)
        goto done;

    /* unlock principal on this KDC */
    kdb->fail_auth_count = 0;

    /* key data and attributes changed, let the database provider know */
    kdb->mask = KADM5_KEY_DATA | KADM5_ATTRIBUTES |
        KADM5_FAIL_AUTH_COUNT;
    /* | KADM5_CPW_FUNCTION */

    ret = k5_kadm5_hook_chpass(handle->context, handle->hook_handles,
                               KADM5_HOOK_STAGE_PRECOMMIT, principal, keepold,
                               new_n_ks_tuple, new_ks_tuple, password);
    if (ret)
        goto done;

    if ((ret = kdb_put_entry(handle, kdb, &adb)))
        goto done;

    (void) k5_kadm5_hook_chpass(handle->context, handle->hook_handles,
                                KADM5_HOOK_STAGE_POSTCOMMIT, principal,
                                keepold, new_n_ks_tuple, new_ks_tuple, password);
    ret = KADM5_OK;
done:
    free(new_ks_tuple);
    if (!hist_added && hist.key_data)
        free_history_entry(handle->context, &hist);
    kdb_free_entry(handle, kdb, &adb);
    kdb_free_keyblocks(handle, hist_keyblocks);

    if (have_pol && (ret2 = kadm5_free_policy_ent(handle->lhandle, &pol))
        && !ret)
        ret = ret2;

    return ret;
}
