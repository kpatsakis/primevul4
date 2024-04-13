static int append_new_servers(GArray *const servers, GError **const gerror) {
        int i;
        GArray *new_servers;
        const int old_len = servers->len;
        int retval = -1;
        struct generic_conf genconf;

        new_servers = parse_cfile(config_file_pos, &genconf, gerror);
        if (!new_servers)
                goto out;

        for (i = 0; i < new_servers->len; ++i) {
                SERVER new_server = g_array_index(new_servers, SERVER, i);

                if (new_server.servename
                    && -1 == get_index_by_servename(new_server.servename,
                                                    servers)) {
                        if (setup_serve(&new_server, gerror) == -1)
                                goto out;
                        if (append_serve(&new_server, servers) == -1)
                                goto out;
                }
        }

        retval = servers->len - old_len;
out:
        g_array_free(new_servers, TRUE);

        return retval;
}
