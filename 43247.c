kadm5_purgekeys(void *server_handle,
                krb5_principal principal,
                int keepkvno)
{
    kadm5_server_handle_t handle = server_handle;
    kadm5_ret_t ret;
    krb5_db_entry *kdb;
    osa_princ_ent_rec adb;
    krb5_key_data *old_keydata;
    int n_old_keydata;
    int i, j, k;

    CHECK_HANDLE(server_handle);

    if (principal == NULL)
        return EINVAL;

    ret = kdb_get_entry(handle, principal, &kdb, &adb);
    if (ret)
        return(ret);

    if (keepkvno <= 0) {
        keepkvno = krb5_db_get_key_data_kvno(handle->context, kdb->n_key_data,
                                             kdb->key_data);
    }

    old_keydata = kdb->key_data;
    n_old_keydata = kdb->n_key_data;
    kdb->n_key_data = 0;
    /* Allocate one extra key_data to avoid allocating 0 bytes. */
    kdb->key_data = krb5_db_alloc(handle->context, NULL,
                                  (n_old_keydata + 1) * sizeof(krb5_key_data));
    if (kdb->key_data == NULL) {
        ret = ENOMEM;
        goto done;
    }
    memset(kdb->key_data, 0, n_old_keydata * sizeof(krb5_key_data));
    for (i = 0, j = 0; i < n_old_keydata; i++) {
        if (old_keydata[i].key_data_kvno < keepkvno)
            continue;

        /* Alias the key_data_contents pointers; we null them out in the
         * source array immediately after. */
        kdb->key_data[j] = old_keydata[i];
        for (k = 0; k < old_keydata[i].key_data_ver; k++) {
            old_keydata[i].key_data_contents[k] = NULL;
        }
        j++;
    }
    kdb->n_key_data = j;
    cleanup_key_data(handle->context, n_old_keydata, old_keydata);

    kdb->mask = KADM5_KEY_DATA;
    ret = kdb_put_entry(handle, kdb, &adb);
    if (ret)
        goto done;

done:
    kdb_free_entry(handle, kdb, &adb);
    return ret;
}
