get_data(char *buf, int len)
{
    char *data;
    int pos = 0;

    while (pos < len && buf[pos] != '{')
        pos++;
    if (pos == len) {
        return NULL;
    }
    data = buf + pos - 1;

    return data;
}

static char *
get_action(char *buf, int len)
{
    char *action;
    int pos = 0;

    while (pos < len && isspace((unsigned char)buf[pos]))
        pos++;
    if (pos == len) {
        return NULL;
    }
    action = buf + pos;

    while (pos < len && (!isspace((unsigned char)buf[pos]) && buf[pos] != ':'))
        pos++;
    buf[pos] = '\0';

    return action;
}

static struct server *
get_server(char *buf, int len)
{
    char *data = get_data(buf, len);
    char error_buf[512];

    if (data == NULL) {
        LOGE("No data found");
        return NULL;
    }

    json_settings settings = { 0 };
    json_value *obj        = json_parse_ex(&settings, data, strlen(data), error_buf);

    if (obj == NULL) {
        LOGE("%s", error_buf);
        return NULL;
    }

    struct server *server = ss_malloc(sizeof(struct server));
    memset(server, 0, sizeof(struct server));
    if (obj->type == json_object) {
        int i = 0;
        for (i = 0; i < obj->u.object.length; i++) {
            char *name        = obj->u.object.values[i].name;
            json_value *value = obj->u.object.values[i].value;
            if (strcmp(name, "server_port") == 0) {
                if (value->type == json_string) {
                    strncpy(server->port, value->u.string.ptr, 8);
                } else if (value->type == json_integer) {
                    snprintf(server->port, 8, "%" PRIu64 "", value->u.integer);
                }
            } else if (strcmp(name, "password") == 0) {
                if (value->type == json_string) {
                    strncpy(server->password, value->u.string.ptr, 128);
                }
            } else if (strcmp(name, "method") == 0) {
                if (value->type == json_string) {
                    server->method = strdup(value->u.string.ptr);
                }
            } else if (strcmp(name, "fast_open") == 0) {
                if (value->type == json_boolean) {
                    strncpy(server->fast_open, (value->u.boolean ? "true" : "false"), 8);
                }
            } else if (strcmp(name, "plugin") == 0) {
                if (value->type == json_string) {
                    server->plugin = strdup(value->u.string.ptr);
                }
            } else if (strcmp(name, "plugin_opts") == 0) {
                if (value->type == json_string) {
                    server->plugin_opts = strdup(value->u.string.ptr);
                }
            } else if (strcmp(name, "mode") == 0) {
                if (value->type == json_string) {
                    server->mode = strdup(value->u.string.ptr);
                }
            } else {
                LOGE("invalid data: %s", data);
                break;
            }
        }
    }

    json_value_free(obj);
    return server;
}

static int
parse_traffic(char *buf, int len, char *port, uint64_t *traffic)
{
    char *data = get_data(buf, len);
    char error_buf[512];
    json_settings settings = { 0 };

    if (data == NULL) {
        LOGE("No data found");
        return -1;
    }

    json_value *obj = json_parse_ex(&settings, data, strlen(data), error_buf);
    if (obj == NULL) {
        LOGE("%s", error_buf);
        return -1;
    }

    if (obj->type == json_object) {
        int i = 0;
        for (i = 0; i < obj->u.object.length; i++) {
            char *name        = obj->u.object.values[i].name;
            json_value *value = obj->u.object.values[i].value;
            if (value->type == json_integer) {
                strncpy(port, name, 8);
                *traffic = value->u.integer;
            }
        }
    }

    json_value_free(obj);
    return 0;
}

static int
create_and_bind(const char *host, const char *port, int protocol)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp, *ipv4v6bindall;
    int s, listen_sock = -1;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = AF_UNSPEC;                  /* Return IPv4 and IPv6 choices */
    hints.ai_socktype = protocol == IPPROTO_TCP ?
                        SOCK_STREAM : SOCK_DGRAM;   /* We want a TCP or UDP socket */
    hints.ai_flags    = AI_PASSIVE | AI_ADDRCONFIG; /* For wildcard IP address */
    hints.ai_protocol = protocol;

    s = getaddrinfo(host, port, &hints, &result);

    if (s != 0) {
        LOGE("getaddrinfo: %s", gai_strerror(s));
        return -1;
    }

    rp = result;

    /*
     * On Linux, with net.ipv6.bindv6only = 0 (the default), getaddrinfo(NULL) with
     * AI_PASSIVE returns 0.0.0.0 and :: (in this order). AI_PASSIVE was meant to
     * return a list of addresses to listen on, but it is impossible to listen on
     * 0.0.0.0 and :: at the same time, if :: implies dualstack mode.
     */
    if (!host) {
        ipv4v6bindall = result;

        /* Loop over all address infos found until a IPV6 address is found. */
        while (ipv4v6bindall) {
            if (ipv4v6bindall->ai_family == AF_INET6) {
                rp = ipv4v6bindall; /* Take first IPV6 address available */
                break;
            }
            ipv4v6bindall = ipv4v6bindall->ai_next; /* Get next address info, if any */
        }
    }

    for (/*rp = result*/; rp != NULL; rp = rp->ai_next) {
        listen_sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (listen_sock == -1) {
            continue;
        }

        if (rp->ai_family == AF_INET6) {
            int ipv6only = host ? 1 : 0;
            setsockopt(listen_sock, IPPROTO_IPV6, IPV6_V6ONLY, &ipv6only, sizeof(ipv6only));
        }

        int opt = 1;
        setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#ifdef SO_NOSIGPIPE
        setsockopt(listen_sock, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt));
#endif

        s = bind(listen_sock, rp->ai_addr, rp->ai_addrlen);
        if (s == 0) {
            /* We managed to bind successfully! */

            close(listen_sock);

            break;
        } else {
            ERROR("bind");
        }
    }

    if (!result) {
        freeaddrinfo(result);
    }

    if (rp == NULL) {
        LOGE("Could not bind");
        return -1;
    }

    return listen_sock;
}

static int
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

static int
add_server(struct manager_ctx *manager, struct server *server)
{
    int ret = check_port(manager, server);

    if (ret == -1) {
        LOGE("port is not available, please check.");
        return -1;
    }

    bool new = false;
    cork_hash_table_put(server_table, (void *)server->port, (void *)server, &new, NULL, NULL);

    char *cmd = construct_command_line(manager, server);
    if (system(cmd) == -1) {
        ERROR("add_server_system");
        return -1;
    }

    return 0;
}

static void
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

static void
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

static void
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

static void
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

static void
manager_recv_cb(EV_P_ ev_io *w, int revents)
{
    struct manager_ctx *manager = (struct manager_ctx *)w;
    socklen_t len;
    ssize_t r;
    struct sockaddr_un claddr;
    char buf[BUF_SIZE];

    memset(buf, 0, BUF_SIZE);

    len = sizeof(struct sockaddr_un);
    r   = recvfrom(manager->fd, buf, BUF_SIZE, 0, (struct sockaddr *)&claddr, &len);
    if (r == -1) {
        ERROR("manager_recvfrom");
        return;
    }

    if (r > BUF_SIZE / 2) {
        LOGE("too large request: %d", (int)r);
        return;
    }

    char *action = get_action(buf, r);
    if (action == NULL) {
        return;
    }

    if (strcmp(action, "add") == 0) {
        struct server *server = get_server(buf, r);

        if (server == NULL || server->port[0] == 0 || server->password[0] == 0) {
            LOGE("invalid command: %s:%s", buf, get_data(buf, r));
            if (server != NULL) {
                destroy_server(server);
                ss_free(server);
            }
            goto ERROR_MSG;
        }

        remove_server(working_dir, server->port);
        int ret = add_server(manager, server);

        char *msg;
        int msg_len;

        if (ret == -1) {
            msg     = "port is not available";
            msg_len = 21;
        } else {
            msg     = "ok";
            msg_len = 2;
        }

        if (sendto(manager->fd, msg, msg_len, 0, (struct sockaddr *)&claddr, len) != 2) {
            ERROR("add_sendto");
        }
    } else if (strcmp(action, "list") == 0) {
        struct cork_hash_table_iterator  iter;
        struct cork_hash_table_entry  *entry;
        char buf[BUF_SIZE];
        memset(buf, 0, BUF_SIZE);
        sprintf(buf, "[");

        cork_hash_table_iterator_init(server_table, &iter);
        while ((entry = cork_hash_table_iterator_next(&iter)) != NULL) {
            struct server *server = (struct server *)entry->value;
            char *method = server->method?server->method:manager->method;
            size_t pos = strlen(buf);
            size_t entry_len = strlen(server->port) + strlen(server->password) + strlen(method);
            if (pos > BUF_SIZE-entry_len-50) {
                if (sendto(manager->fd, buf, pos, 0, (struct sockaddr *)&claddr, len)
                    != pos) {
                    ERROR("list_sendto");
                }
                memset(buf, 0, BUF_SIZE);
                pos = 0;
            }
            sprintf(buf + pos, "\n\t{\"server_port\":\"%s\",\"password\":\"%s\",\"method\":\"%s\"},", 
                    server->port,server->password,method);

        }

        size_t pos = strlen(buf);
        strcpy(buf + pos - 1, "\n]"); //Remove trailing ","
        pos = strlen(buf);
        if (sendto(manager->fd, buf, pos, 0, (struct sockaddr *)&claddr, len)
            != pos) {
            ERROR("list_sendto");
        }
    } else if (strcmp(action, "remove") == 0) {
        struct server *server = get_server(buf, r);

        if (server == NULL || server->port[0] == 0) {
            LOGE("invalid command: %s:%s", buf, get_data(buf, r));
            if (server != NULL) {
                destroy_server(server);
                ss_free(server);
            }
            goto ERROR_MSG;
        }

        remove_server(working_dir, server->port);
        destroy_server(server);
        ss_free(server);

        char msg[3] = "ok";
        if (sendto(manager->fd, msg, 2, 0, (struct sockaddr *)&claddr, len) != 2) {
            ERROR("remove_sendto");
        }
    } else if (strcmp(action, "stat") == 0) {
        char port[8];
        uint64_t traffic = 0;

        if (parse_traffic(buf, r, port, &traffic) == -1) {
            LOGE("invalid command: %s:%s", buf, get_data(buf, r));
            return;
        }

        update_stat(port, traffic);
    } else if (strcmp(action, "ping") == 0) {
        struct cork_hash_table_entry *entry;
        struct cork_hash_table_iterator server_iter;

        char buf[BUF_SIZE];

        memset(buf, 0, BUF_SIZE);
        sprintf(buf, "stat: {");

        cork_hash_table_iterator_init(server_table, &server_iter);

        while ((entry = cork_hash_table_iterator_next(&server_iter)) != NULL) {
            struct server *server = (struct server *)entry->value;
            size_t pos            = strlen(buf);
            if (pos > BUF_SIZE / 2) {
                buf[pos - 1] = '}';
                if (sendto(manager->fd, buf, pos, 0, (struct sockaddr *)&claddr, len)
                    != pos) {
                    ERROR("ping_sendto");
                }
                memset(buf, 0, BUF_SIZE);
            } else {
                sprintf(buf + pos, "\"%s\":%" PRIu64 ",", server->port, server->traffic);
            }
        }

        size_t pos = strlen(buf);
        if (pos > 7) {
            buf[pos - 1] = '}';
        } else {
            buf[pos] = '}';
            pos++;
        }

        if (sendto(manager->fd, buf, pos, 0, (struct sockaddr *)&claddr, len)
            != pos) {
            ERROR("ping_sendto");
        }
    }
