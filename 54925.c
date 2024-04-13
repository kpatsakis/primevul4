kill_server(char *prefix, char *pid_file)
{
    char *path = NULL;
    int pid, path_size = strlen(prefix) + strlen(pid_file) + 2;
    path = ss_malloc(path_size);
    snprintf(path, path_size, "%s/%s", prefix, pid_file);
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
    remove(path);
    ss_free(path);
}
