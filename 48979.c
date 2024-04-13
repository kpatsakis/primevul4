copy_keyblock_to_lucid_key(
    krb5_keyblock *k5key,
    gss_krb5_lucid_key_t *lkey)
{
    if (!k5key || !k5key->contents || k5key->length == 0)
        return EINVAL;

    memset(lkey, 0, sizeof(gss_krb5_lucid_key_t));

    /* Allocate storage for the key data */
    if ((lkey->data = xmalloc(k5key->length)) == NULL) {
        return ENOMEM;
    }
    memcpy(lkey->data, k5key->contents, k5key->length);
    lkey->length = k5key->length;
    lkey->type = k5key->enctype;

    return 0;
}
