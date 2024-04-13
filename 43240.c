apply_keysalt_policy(kadm5_server_handle_t handle, const char *policy,
                     int n_ks_tuple, krb5_key_salt_tuple *ks_tuple,
                     int *new_n_kstp, krb5_key_salt_tuple **new_kstp)
{
    kadm5_ret_t ret;
    kadm5_policy_ent_rec polent;
    krb5_boolean have_polent;
    int ak_n_ks_tuple = 0;
    int new_n_ks_tuple = 0;
    krb5_key_salt_tuple *ak_ks_tuple = NULL;
    krb5_key_salt_tuple *new_ks_tuple = NULL;
    krb5_key_salt_tuple *subset;
    int i, m;

    if (new_n_kstp != NULL) {
        *new_n_kstp = 0;
        *new_kstp = NULL;
    }

    memset(&polent, 0, sizeof(polent));
    ret = get_policy(handle, policy, &polent, &have_polent);
    if (ret)
        goto cleanup;

    if (polent.allowed_keysalts == NULL) {
        /* Requested keysalts allowed or default to supported_enctypes. */
        if (n_ks_tuple == 0) {
            /* Default to supported_enctypes. */
            n_ks_tuple = handle->params.num_keysalts;
            ks_tuple = handle->params.keysalts;
        }
        /* Dup the requested or defaulted keysalt tuples. */
        new_ks_tuple = malloc(n_ks_tuple * sizeof(*new_ks_tuple));
        if (new_ks_tuple == NULL) {
            ret = ENOMEM;
            goto cleanup;
        }
        memcpy(new_ks_tuple, ks_tuple, n_ks_tuple * sizeof(*new_ks_tuple));
        new_n_ks_tuple = n_ks_tuple;
        ret = 0;
        goto cleanup;
    }

    ret = krb5_string_to_keysalts(polent.allowed_keysalts,
                                  ",",   /* Tuple separators */
                                  NULL,  /* Key/salt separators */
                                  0,     /* No duplicates */
                                  &ak_ks_tuple,
                                  &ak_n_ks_tuple);
    /*
     * Malformed policy?  Shouldn't happen, but it's remotely possible
     * someday, so we don't assert, just bail.
     */
    if (ret)
        goto cleanup;

    /* Check that the requested ks_tuples are within policy, if we have one. */
    for (i = 0; i < n_ks_tuple; i++) {
        if (!ks_tuple_present(ak_n_ks_tuple, ak_ks_tuple, &ks_tuple[i])) {
            ret = KADM5_BAD_KEYSALTS;
            goto cleanup;
        }
    }

    /* Have policy but no ks_tuple input?  Output the policy. */
    if (n_ks_tuple == 0) {
        new_n_ks_tuple = ak_n_ks_tuple;
        new_ks_tuple = ak_ks_tuple;
        ak_ks_tuple = NULL;
        goto cleanup;
    }

    /*
     * Now filter the policy ks tuples by the requested ones so as to
     * preserve in the requested sub-set the relative ordering from the
     * policy.  We could optimize this (if (n_ks_tuple == ak_n_ks_tuple)
     * then skip this), but we don't bother.
     */
    subset = calloc(n_ks_tuple, sizeof(*subset));
    if (subset == NULL) {
        ret = ENOMEM;
        goto cleanup;
    }
    for (m = 0, i = 0; i < ak_n_ks_tuple && m < n_ks_tuple; i++) {
        if (ks_tuple_present(n_ks_tuple, ks_tuple, &ak_ks_tuple[i]))
            subset[m++] = ak_ks_tuple[i];
    }
    new_ks_tuple = subset;
    new_n_ks_tuple = m;
    ret = 0;

cleanup:
    if (have_polent)
        kadm5_free_policy_ent(handle->lhandle, &polent);
    free(ak_ks_tuple);

    if (new_n_kstp != NULL) {
        *new_n_kstp = new_n_ks_tuple;
        *new_kstp = new_ks_tuple;
    } else {
        free(new_ks_tuple);
    }
    return ret;
}
