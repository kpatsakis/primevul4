krb5_decode_krbsecretkey(krb5_context context, krb5_db_entry *entries,
                         struct berval **bvalues, krb5_kvno *mkvno)
{
    krb5_key_data *key_data = NULL, *tmp;
    krb5_error_code err = 0;
    ldap_seqof_key_data *keysets = NULL;
    krb5_int16 i, n_keysets = 0, total_keys = 0;

    err = decode_keys(bvalues, &keysets, &n_keysets, &total_keys);
    if (err != 0) {
        k5_prependmsg(context, err,
                      _("unable to decode stored principal key data"));
        goto cleanup;
    }

    key_data = k5calloc(total_keys, sizeof(krb5_key_data), &err);
    if (key_data == NULL)
        goto cleanup;
    memset(key_data, 0, total_keys * sizeof(krb5_key_data));

    if (n_keysets > 0)
        *mkvno = keysets[0].mkvno;

    /* Transfer key data values from keysets to a flat list in entries. */
    tmp = key_data;
    for (i = 0; i < n_keysets; i++) {
        memcpy(tmp, keysets[i].key_data,
               sizeof(krb5_key_data) * keysets[i].n_key_data);
        tmp += keysets[i].n_key_data;
        keysets[i].n_key_data = 0;
    }
    entries->n_key_data = total_keys;
    entries->key_data = key_data;
    key_data = NULL;

cleanup:
    free_ldap_seqof_key_data(keysets, n_keysets);
    k5_free_key_data(total_keys, key_data);
    return err;
}
