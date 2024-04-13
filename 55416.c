static int racl_cb(void *rock,
                   const char *key, size_t keylen,
                   const char *data __attribute__((unused)),
                   size_t datalen __attribute__((unused)))
{
    struct raclrock *raclrock = (struct raclrock *)rock;
    strarray_appendm(raclrock->list, xstrndup(key + raclrock->prefixlen, keylen - raclrock->prefixlen));
    return 0;
}
