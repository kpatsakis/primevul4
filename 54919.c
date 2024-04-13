check_port(struct manager_ctx *manager, struct server *server)
{
    bool both_tcp_udp = manager->mode == TCP_AND_UDP;
    int fd_count      = manager->host_num * (both_tcp_udp ? 2 : 1);
    int bind_err      = 0;

    int *sock_fds = (int *)ss_malloc(fd_count * sizeof(int));
    memset(sock_fds, 0, fd_count * sizeof(int));

    /* try to bind each interface */
    for (int i = 0; i < manager->host_num; i++) {
        LOGI("try to bind interface: %s, port: %s", manager->hosts[i], server->port);

        if (manager->mode == UDP_ONLY) {
            sock_fds[i] = create_and_bind(manager->hosts[i], server->port, IPPROTO_UDP);
        } else {
            sock_fds[i] = create_and_bind(manager->hosts[i], server->port, IPPROTO_TCP);
        }

        if (both_tcp_udp) {
            sock_fds[i + manager->host_num] = create_and_bind(manager->hosts[i], server->port, IPPROTO_UDP);
        }

        if (sock_fds[i] == -1 || (both_tcp_udp && sock_fds[i + manager->host_num] == -1)) {
            bind_err = -1;
            break;
        }
    }

    /* clean socks */
    for (int i = 0; i < fd_count; i++) {
        if (sock_fds[i] > 0) {
            close(sock_fds[i]);
        }
    }

    ss_free(sock_fds);

    return bind_err == -1 ? -1 : 0;
}
