remove_server(char *prefix, char *port)
{
    char *old_port            = NULL;
    struct server *old_server = NULL;

    cork_hash_table_delete(server_table, (void *)port, (void **)&old_port, (void **)&old_server);

    if (old_server != NULL) {
        destroy_server(old_server);
        ss_free(old_server);
    }

    stop_server(prefix, port);
}
