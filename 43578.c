krb5_decode_krbsecretkey(krb5_context context, krb5_db_entry *entries,
                         struct berval **bvalues,
                         krb5_tl_data *userinfo_tl_data, krb5_kvno *mkvno)
{
    char                        *user=NULL;
    int                         i=0, j=0, noofkeys=0;
    krb5_key_data               *key_data=NULL, *tmp;
    krb5_error_code             st=0;

    if ((st=krb5_unparse_name(context, entries->princ, &user)) != 0)
        goto cleanup;

    for (i=0; bvalues[i] != NULL; ++i) {
        krb5_int16 n_kd;
        krb5_key_data *kd;
        krb5_data in;

        if (bvalues[i]->bv_len == 0)
            continue;
        in.length = bvalues[i]->bv_len;
        in.data = bvalues[i]->bv_val;

        st = asn1_decode_sequence_of_keys (&in,
                                           &kd,
                                           &n_kd,
                                           mkvno);

        if (st != 0) {
            const char *msg = error_message(st);
            st = -1; /* Something more appropriate ? */
            krb5_set_error_message(context, st, _("unable to decode stored "
                                                  "principal key data (%s)"),
                                   msg);
            goto cleanup;
        }
        noofkeys += n_kd;
        tmp = key_data;
        /* Allocate an extra key data to avoid allocating zero bytes. */
        key_data = realloc(key_data, (noofkeys + 1) * sizeof (krb5_key_data));
        if (key_data == NULL) {
            key_data = tmp;
            st = ENOMEM;
            goto cleanup;
        }
        for (j = 0; j < n_kd; j++)
            key_data[noofkeys - n_kd + j] = kd[j];
        free (kd);
    }

    entries->n_key_data = noofkeys;
    entries->key_data = key_data;

cleanup:
    ldap_value_free_len(bvalues);
    free (user);
    return st;
}
