krb5_encode_histkey(osa_princ_ent_rec *princ_ent)
{
    unsigned int i;
    krb5_error_code err = 0;
    struct berval **ret = NULL;

    if (princ_ent->old_key_len <= 0)
        return NULL;

    ret = k5calloc(princ_ent->old_key_len + 1, sizeof(struct berval *), &err);
    if (ret == NULL)
        goto cleanup;

    for (i = 0; i < princ_ent->old_key_len; i++) {
        if (princ_ent->old_keys[i].n_key_data <= 0) {
            err = EINVAL;
            goto cleanup;
        }
        err = encode_keys(princ_ent->old_keys[i].key_data,
                          princ_ent->old_keys[i].n_key_data,
                          princ_ent->admin_history_kvno, &ret[i]);
        if (err)
            goto cleanup;
    }

    ret[princ_ent->old_key_len] = NULL;

cleanup:
    if (err != 0) {
        free_berdata(ret);
        ret = NULL;
    }

    return ret;
}
