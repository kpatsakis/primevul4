update_stat(char *port, uint64_t traffic)
{
    if (verbose) {
        LOGI("update traffic %" PRIu64 " for port %s", traffic, port);
    }
    void *ret = cork_hash_table_get(server_table, (void *)port);
    if (ret != NULL) {
        struct server *server = (struct server *)ret;
        server->traffic = traffic;
    }
}
