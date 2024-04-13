static int mk_security_check_ip(int socket)
{
    int network;
    struct mk_secure_ip_t *entry;
    struct mk_list *head;
    struct in_addr addr_t, *addr = &addr_t;
    socklen_t len = sizeof(addr);

    if (getpeername(socket, (struct sockaddr *)&addr_t, &len) < 0) {
        return -1;
    }

    PLUGIN_TRACE("[FD %i] Mandril validating IP address", socket);
    mk_list_foreach(head, &mk_secure_ip) {
        entry = mk_list_entry(head, struct mk_secure_ip_t, _head);

        if (entry->is_subnet == MK_TRUE) {
            /* Validate network */
            network = MK_NET_NETWORK(addr->s_addr, entry->netmask);
            if (network != entry->network) {
                continue;
            }

            /* Validate host range */
            if (addr->s_addr <= entry->hostmax && addr->s_addr >= entry->hostmin) {
                PLUGIN_TRACE("[FD %i] Mandril closing by rule in ranges", socket);
                return -1;
            }
        }
        else {
            if (addr->s_addr == entry->ip.s_addr) {
                PLUGIN_TRACE("[FD %i] Mandril closing by rule in IP match", socket);
                return -1;
            }
        }
    }
    return 0;
}
