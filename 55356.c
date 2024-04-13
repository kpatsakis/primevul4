static int exists_cb(const mbentry_t *mbentry __attribute__((unused)), void *rock)
{
    int *exists = (int *)rock;
    *exists = 1;
    return CYRUSDB_DONE; /* one is enough */
}
