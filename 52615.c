krb5_decode_histkey(krb5_context context, struct berval **bvalues,
                    osa_princ_ent_rec *princ_ent)
{
    krb5_error_code err = 0;
    krb5_int16 i, n_keysets = 0;
    ldap_seqof_key_data *keysets = NULL;

    err = decode_keys(bvalues, &keysets, &n_keysets, NULL);
    if (err != 0) {
        k5_prependmsg(context, err,
                      _("unable to decode stored principal pw history"));
        goto cleanup;
    }

    princ_ent->old_keys = k5calloc(n_keysets, sizeof(osa_pw_hist_ent), &err);
    if (princ_ent->old_keys == NULL)
        goto cleanup;
    princ_ent->old_key_len = n_keysets;

    if (n_keysets > 0)
        princ_ent->admin_history_kvno = keysets[0].mkvno;

    /* Transfer key data pointers from keysets to princ_ent. */
    for (i = 0; i < n_keysets; i++) {
        princ_ent->old_keys[i].n_key_data = keysets[i].n_key_data;
        princ_ent->old_keys[i].key_data = keysets[i].key_data;
        keysets[i].n_key_data = 0;
        keysets[i].key_data = NULL;
    }

    /* Sort the principal entries by kvno in ascending order. */
    qsort(princ_ent->old_keys, princ_ent->old_key_len, sizeof(osa_pw_hist_ent),
          &compare_osa_pw_hist_ent);

    princ_ent->aux_attributes |= KADM5_KEY_HIST;

    /* Set the next key to the end of the list.  The queue will be lengthened
     * if it isn't full yet; the first entry will be replaced if it is full. */
    princ_ent->old_key_next = princ_ent->old_key_len;

cleanup:
    free_ldap_seqof_key_data(keysets, n_keysets);
    return err;
}
