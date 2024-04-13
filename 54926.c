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
