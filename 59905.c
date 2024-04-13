rend_service_parse_port_config(const char *string, const char *sep,
                               char **err_msg_out)
{
  smartlist_t *sl;
  int virtport;
  int realport = 0;
  uint16_t p;
  tor_addr_t addr;
  rend_service_port_config_t *result = NULL;
  unsigned int is_unix_addr = 0;
  const char *socket_path = NULL;
  char *err_msg = NULL;
  char *addrport = NULL;

  sl = smartlist_new();
  smartlist_split_string(sl, string, sep,
                         SPLIT_SKIP_SPACE|SPLIT_IGNORE_BLANK, 2);
  if (smartlist_len(sl) < 1 || BUG(smartlist_len(sl) > 2)) {
    err_msg = tor_strdup("Bad syntax in hidden service port configuration.");
    goto err;
  }
  virtport = (int)tor_parse_long(smartlist_get(sl,0), 10, 1, 65535, NULL,NULL);
  if (!virtport) {
    tor_asprintf(&err_msg, "Missing or invalid port %s in hidden service "
                   "port configuration", escaped(smartlist_get(sl,0)));

    goto err;
  }
  if (smartlist_len(sl) == 1) {
    /* No addr:port part; use default. */
    realport = virtport;
    tor_addr_from_ipv4h(&addr, 0x7F000001u); /* 127.0.0.1 */
  } else {
    int ret;

    const char *addrport_element = smartlist_get(sl,1);
    const char *rest = NULL;
    int is_unix;
    ret = port_cfg_line_extract_addrport(addrport_element, &addrport,
                                         &is_unix, &rest);
    if (ret < 0) {
      tor_asprintf(&err_msg, "Couldn't process address <%s> from hidden "
                   "service configuration", addrport_element);
      goto err;
    }
    if (is_unix) {
      socket_path = addrport;
      is_unix_addr = 1;
    } else if (strchr(addrport, ':') || strchr(addrport, '.')) {
      /* else try it as an IP:port pair if it has a : or . in it */
      if (tor_addr_port_lookup(addrport, &addr, &p)<0) {
        err_msg = tor_strdup("Unparseable address in hidden service port "
                             "configuration.");
        goto err;
      }
      realport = p?p:virtport;
    } else {
      /* No addr:port, no addr -- must be port. */
      realport = (int)tor_parse_long(addrport, 10, 1, 65535, NULL, NULL);
      if (!realport) {
        tor_asprintf(&err_msg, "Unparseable or out-of-range port %s in "
                     "hidden service port configuration.",
                     escaped(addrport));
        goto err;
      }
      tor_addr_from_ipv4h(&addr, 0x7F000001u); /* Default to 127.0.0.1 */
    }
  }

  /* Allow room for unix_addr */
  result = rend_service_port_config_new(socket_path);
  result->virtual_port = virtport;
  result->is_unix_addr = is_unix_addr;
  if (!is_unix_addr) {
    result->real_port = realport;
    tor_addr_copy(&result->real_addr, &addr);
    result->unix_addr[0] = '\0';
  }

 err:
  tor_free(addrport);
  if (err_msg_out != NULL) {
    *err_msg_out = err_msg;
  } else {
    tor_free(err_msg);
  }
  SMARTLIST_FOREACH(sl, char *, c, tor_free(c));
  smartlist_free(sl);

  return result;
}
