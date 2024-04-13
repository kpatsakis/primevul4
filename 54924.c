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
