make_ks_from_keys(krb5_context context, int n_keys, krb5_keyblock *keyblocks,
                  krb5_key_salt_tuple **ks_tuple)
{
    int i;

    *ks_tuple = calloc(n_keys, sizeof(**ks_tuple));
    if (*ks_tuple == NULL)
        return ENOMEM;

    for (i = 0; i < n_keys; i++) {
        (*ks_tuple)[i].ks_enctype = keyblocks[i].enctype;
        (*ks_tuple)[i].ks_salttype = KRB5_KDB_SALTTYPE_NORMAL;
    }
    return 0;
}
