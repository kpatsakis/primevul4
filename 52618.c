krb5_encode_krbsecretkey(krb5_key_data *key_data, int n_key_data,
                         krb5_kvno mkvno)
{
    struct berval **ret = NULL;
    int currkvno;
    int num_versions = 0;
    int i, j, last;
    krb5_error_code err = 0;

    if (n_key_data < 0)
        return NULL;

    /* Find the number of key versions */
    if (n_key_data > 0) {
        for (i = 0, num_versions = 1; i < n_key_data - 1; i++) {
            if (key_data[i].key_data_kvno != key_data[i + 1].key_data_kvno)
                num_versions++;
        }
    }

    ret = calloc(num_versions + 1, sizeof(struct berval *));
    if (ret == NULL) {
        err = ENOMEM;
        goto cleanup;
    }
    ret[num_versions] = NULL;

    /* n_key_data may be 0 if a principal is created without a key. */
    if (n_key_data == 0)
        goto cleanup;

    currkvno = key_data[0].key_data_kvno;
    for (i = 0, last = 0, j = 0; i < n_key_data; i++) {
        if (i == n_key_data - 1 || key_data[i + 1].key_data_kvno != currkvno) {
            err = encode_keys(key_data + last, (krb5_int16)i - last + 1, mkvno,
                              &ret[j]);
            if (err)
                goto cleanup;
            j++;
            last = i + 1;

            if (i < n_key_data - 1)
                currkvno = key_data[i + 1].key_data_kvno;
        }
    }

cleanup:
    if (err != 0) {
        free_berdata(ret);
        ret = NULL;
    }

    return ret;
}
