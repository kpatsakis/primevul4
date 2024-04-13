static int racls_del_cb(void *rock,
                  const char *key, size_t keylen,
                  const char *data __attribute__((unused)),
                  size_t datalen __attribute__((unused)))
{
    struct txn **txn = (struct txn **)rock;
    return cyrusdb_delete(mbdb, key, keylen, txn, /*force*/0);
}
