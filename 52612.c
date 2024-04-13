decode_keys(struct berval **bvalues, ldap_seqof_key_data **keysets_out,
            krb5_int16 *n_keysets_out, krb5_int16 *total_keys_out)
{
    krb5_error_code err = 0;
    krb5_int16 n_keys, i, ki, total_keys;
    ldap_seqof_key_data *keysets = NULL;

    *keysets_out = NULL;
    *n_keysets_out = 0;
    if (total_keys_out)
        *total_keys_out = 0;

    /* Precount the number of keys. */
    for (n_keys = 0, i = 0; bvalues[i] != NULL; i++) {
        if (bvalues[i]->bv_len > 0)
            n_keys++;
    }

    keysets = k5calloc(n_keys, sizeof(ldap_seqof_key_data), &err);
    if (keysets == NULL)
        goto cleanup;
    memset(keysets, 0, n_keys * sizeof(ldap_seqof_key_data));

    for (i = 0, ki = 0, total_keys = 0; bvalues[i] != NULL; i++) {
        krb5_data in;

        if (bvalues[i]->bv_len == 0)
            continue;
        in.length = bvalues[i]->bv_len;
        in.data = bvalues[i]->bv_val;

        err = asn1_decode_sequence_of_keys(&in, &keysets[ki]);
        if (err)
            goto cleanup;

        if (total_keys_out)
            total_keys += keysets[ki].n_key_data;
        ki++;
    }

    if (total_keys_out)
        *total_keys_out = total_keys;

    *n_keysets_out = n_keys;
    *keysets_out = keysets;
    keysets = NULL;
    n_keys = 0;

cleanup:
    free_ldap_seqof_key_data(keysets, n_keys);
    return err;
}
