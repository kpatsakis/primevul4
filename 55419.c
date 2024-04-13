static int subsadd_cb(void *rock, const char *key, size_t keylen,
                      const char *val __attribute__((unused)),
                      size_t vallen __attribute__((unused)))
{
    strarray_t *list = (strarray_t *)rock;
    strarray_appendm(list, xstrndup(key, keylen));
    return 0;
}
