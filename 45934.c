static int mk_security_check_url(mk_ptr_t url)
{
    int n;
    struct mk_list *head;
    struct mk_secure_url_t *entry;

    mk_list_foreach(head, &mk_secure_url) {
        entry = mk_list_entry(head, struct mk_secure_url_t, _head);
        n = mk_api->str_search_n(url.data, entry->criteria, MK_STR_INSENSITIVE, url.len);
        if (n >= 0) {
            return -1;
        }
    }

    return 0;
}
