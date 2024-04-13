count_new_keys(int n_key_data, krb5_key_data *key_data)
{
    int n;

    for (n = 1; n < n_key_data; n++) {
        if (key_data[n - 1].key_data_kvno != key_data[n].key_data_kvno)
            return n;
    }
    return n_key_data;
}
