static int test_same_origin(const char *src, const char *ref) {
    char src_proto[64];
    char ref_proto[64];
    char src_auth[256];
    char ref_auth[256];
    char src_host[256];
    char ref_host[256];
    int src_port=-1;
    int ref_port=-1;

    av_url_split(src_proto, sizeof(src_proto), src_auth, sizeof(src_auth), src_host, sizeof(src_host), &src_port, NULL, 0, src);
    av_url_split(ref_proto, sizeof(ref_proto), ref_auth, sizeof(ref_auth), ref_host, sizeof(ref_host), &ref_port, NULL, 0, ref);

    if (strlen(src) == 0) {
        return -1;
    } else if (strlen(src_auth) + 1 >= sizeof(src_auth) ||
        strlen(ref_auth) + 1 >= sizeof(ref_auth) ||
        strlen(src_host) + 1 >= sizeof(src_host) ||
        strlen(ref_host) + 1 >= sizeof(ref_host)) {
        return 0;
    } else if (strcmp(src_proto, ref_proto) ||
               strcmp(src_auth, ref_auth) ||
               strcmp(src_host, ref_host) ||
               src_port != ref_port) {
        return 0;
    } else
        return 1;
}
