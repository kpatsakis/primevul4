static int mk_security_check_hotlink(mk_ptr_t url, mk_ptr_t host,
        mk_ptr_t referer)
{
    mk_ptr_t ref_host = parse_referer_host(referer);
    unsigned int domains_matched = 0;
    int i = 0;
    const char *curA, *curB;
    struct mk_list *head;
    struct mk_secure_deny_hotlink_t *entry;

    if (ref_host.data == NULL) {
        return 0;
    }
    else if (host.data == NULL) {
        mk_err("No host data.");
        return -1;
    }

    mk_list_foreach(head, &mk_secure_url) {
        entry = mk_list_entry(head, struct mk_secure_deny_hotlink_t, _head);
        i = mk_api->str_search_n(url.data, entry->criteria, MK_STR_INSENSITIVE, url.len);
        if (i >= 0) {
            break;
        }
    }
    if (i < 0) {
        return 0;
    }

    curA = host.data + host.len;
    curB = ref_host.data + ref_host.len;

    while (curA > host.data && curB > ref_host.data) {
        i++;
        curA--;
        curB--;

        if ((*curA == '.' && *curB == '.') ||
                curA == host.data || curB == ref_host.data) {
            if (i < 1) {
                break;
            }
            else if (curA == host.data &&
                    !(curB == ref_host.data || *(curB - 1) == '.')) {
                break;
            }
            else if (curB == ref_host.data &&
                    !(curA == host.data || *(curA - 1) == '.')) {
                break;
            }
            else if (strncasecmp(curA, curB, i)) {
                break;
            }
            domains_matched += 1;
            i = 0;
        }
    }

    return domains_matched >= 2 ? 0 : -1;
}
