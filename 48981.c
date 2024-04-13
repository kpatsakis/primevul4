free_lucid_key_data(
    gss_krb5_lucid_key_t *key)
{
    if (key) {
        if (key->data && key->length) {
            memset(key->data, 0, key->length);
            xfree(key->data);
            memset(key, 0, sizeof(gss_krb5_lucid_key_t));
        }
    }
}
