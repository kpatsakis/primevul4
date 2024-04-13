static int get_index_by_servename(const gchar *const servename,
                                  const GArray *const servers) {
        int i;

        for (i = 0; i < servers->len; ++i) {
                const SERVER server = g_array_index(servers, SERVER, i);

                if (strcmp(servename, server.servename) == 0)
                        return i;
        }

        return -1;
}
