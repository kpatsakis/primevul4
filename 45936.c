mk_ptr_t parse_referer_host(mk_ptr_t ref)
{
    unsigned int i, beginHost, endHost;
    mk_ptr_t host;

    host.data = NULL;
    host.len = 0;

    for (i = 0; i < ref.len && !(ref.data[i] == '/' && ref.data[i+1] == '/'); i++);
    if (i == ref.len) {
        goto error;
    }
    beginHost = i + 2;

    for (; i < ref.len && ref.data[i] != '@'; i++);
    if (i < ref.len) {
        beginHost = i + 1;
    }

    for (i = beginHost; i < ref.len && ref.data[i] != ':' && ref.data[i] != '/'; i++);
    endHost = i;

    host.data = ref.data + beginHost;
    host.len = endHost - beginHost;
    return host;
error:
    host.data = NULL;
    host.len = 0;
    return host;
}
