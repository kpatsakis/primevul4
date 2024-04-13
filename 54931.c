stop_server(char *prefix, char *port)
{
    char *path = NULL;
    int pid, path_size = strlen(prefix) + strlen(port) + 20;
    path = ss_malloc(path_size);
    snprintf(path, path_size, "%s/.shadowsocks_%s.pid", prefix, port);
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        if (verbose) {
            LOGE("unable to open pid file");
        }
        ss_free(path);
        return;
    }
    if (fscanf(f, "%d", &pid) != EOF) {
        kill(pid, SIGTERM);
    }
    fclose(f);
    ss_free(path);
}
