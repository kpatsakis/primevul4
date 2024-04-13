ks_tuple_present(int n_ks_tuple, krb5_key_salt_tuple *ks_tuple,
                 krb5_key_salt_tuple *looking_for)
{
    int i;

    for (i = 0; i < n_ks_tuple; i++) {
        if (ks_tuple[i].ks_enctype == looking_for->ks_enctype &&
            ks_tuple[i].ks_salttype == looking_for->ks_salttype)
            return TRUE;
    }
    return FALSE;
}
