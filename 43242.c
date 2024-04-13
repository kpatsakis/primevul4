int create_history_entry(krb5_context context,
                         krb5_keyblock *hist_key, int n_key_data,
                         krb5_key_data *key_data, osa_pw_hist_ent *hist)
{
    krb5_error_code ret;
    krb5_keyblock key;
    krb5_keysalt salt;
    int i;

    hist->key_data = k5calloc(n_key_data, sizeof(krb5_key_data), &ret);
    if (hist->key_data == NULL)
        return ret;

    for (i = 0; i < n_key_data; i++) {
        ret = krb5_dbe_decrypt_key_data(context, NULL, &key_data[i], &key,
                                        &salt);
        if (ret)
            return ret;

        ret = krb5_dbe_encrypt_key_data(context, hist_key, &key, &salt,
                                        key_data[i].key_data_kvno,
                                        &hist->key_data[i]);
        if (ret)
            return ret;

        krb5_free_keyblock_contents(context, &key);
        /* krb5_free_keysalt(context, &salt); */
    }

    hist->n_key_data = n_key_data;
    return 0;
}
