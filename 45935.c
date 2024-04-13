static int mk_security_conf(char *confdir)
{
    int n;
    int ret = 0;
    unsigned long len;
    char *conf_path = NULL;
    char *_net, *_mask;

    struct mk_secure_ip_t *new_ip;
    struct mk_secure_url_t *new_url;
    struct mk_secure_deny_hotlink_t *new_deny_hotlink;

    struct mk_config_section *section;
    struct mk_config_entry *entry;
    struct mk_list *head;

    /* Read configuration */
    mk_api->str_build(&conf_path, &len, "%s/mandril.conf", confdir);
    conf = mk_api->config_create(conf_path);
    section = mk_api->config_section_get(conf, "RULES");


    mk_list_foreach(head, &section->entries) {
        entry = mk_list_entry(head, struct mk_config_entry, _head);

        /* Passing to internal struct */
        if (strcasecmp(entry->key, "IP") == 0) {
            new_ip = mk_api->mem_alloc(sizeof(struct mk_secure_ip_t));
            n = mk_api->str_search(entry->val, "/", 1);

            /* subnet */
            if (n > 0) {
                /* split network addr and netmask */
                _net  = mk_api->str_copy_substr(entry->val, 0, n);
                _mask = mk_api->str_copy_substr(entry->val,
                                                n + 1,
                                                strlen(entry->val));

                /* validations... */
                if (!_net ||  !_mask) {
                    mk_warn("Mandril: cannot parse entry '%s' in RULES section",
                            entry->val);
                    goto ip_next;
                }

                mk_info("network: '%s' mask: '%s'", _net, _mask);

                /* convert ip string to network address */
                if (inet_aton(_net, &new_ip->ip) == 0) {
                    mk_warn("Mandril: invalid ip address '%s' in RULES section",
                            entry->val);
                    goto ip_next;
                }

                /* parse mask */
                new_ip->netmask = strtol(_mask, (char **) NULL, 10);
                if (new_ip->netmask <= 0 || new_ip->netmask >= 32) {
                    mk_warn("Mandril: invalid mask value '%s' in RULES section",
                            entry->val);
                    goto ip_next;
                }

                /* complete struct data */
                new_ip->is_subnet = MK_TRUE;
                new_ip->network = MK_NET_NETWORK(new_ip->ip.s_addr, new_ip->netmask);
                new_ip->hostmin = MK_NET_HOSTMIN(new_ip->ip.s_addr, new_ip->netmask);
                new_ip->hostmax = MK_NET_HOSTMAX(new_ip->ip.s_addr, new_ip->netmask);

                /* link node with main list */
                mk_list_add(&new_ip->_head, &mk_secure_ip);

            /*
             * I know, you were instructed to hate 'goto' statements!, ok, show this
             * code to your teacher and let him blame :P
             */
            ip_next:
                if (_net) {
                    mk_api->mem_free(_net);
                }
                if (_mask) {
                    mk_api->mem_free(_mask);
                }
            }
            else { /* normal IP address */

                /* convert ip string to network address */
                if (inet_aton(entry->val, &new_ip->ip) == 0) {
                    mk_warn("Mandril: invalid ip address '%s' in RULES section",
                            entry->val);
                }
                else {
                    new_ip->is_subnet = MK_FALSE;
                    mk_list_add(&new_ip->_head, &mk_secure_ip);
                }
            }
        }
        else if (strcasecmp(entry->key, "URL") == 0) {
            /* simple allcotion and data association */
            new_url = mk_api->mem_alloc(sizeof(struct mk_secure_url_t));
            new_url->criteria = entry->val;

            /* link node with main list */
            mk_list_add(&new_url->_head, &mk_secure_url);
        }
        else if (strcasecmp(entry->key, "deny_hotlink") == 0) {
            new_deny_hotlink = mk_api->mem_alloc(sizeof(*new_deny_hotlink));
            new_deny_hotlink->criteria = entry->val;

            mk_list_add(&new_deny_hotlink->_head, &mk_secure_deny_hotlink);
        }
    }

    mk_api->mem_free(conf_path);
    return ret;
}
