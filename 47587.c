__xml_acl_mode_test(enum xml_private_flags allowed, enum xml_private_flags requested)
{
    if(is_set(allowed, xpf_acl_deny)) {
        return FALSE;

    } else if(is_set(allowed, requested)) {
        return TRUE;

    } else if(is_set(requested, xpf_acl_read) && is_set(allowed, xpf_acl_write)) {
        return TRUE;

    } else if(is_set(requested, xpf_acl_create) && is_set(allowed, xpf_acl_write)) {
        return TRUE;

    } else if(is_set(requested, xpf_acl_create) && is_set(allowed, xpf_created)) {
        return TRUE;
    }
    return FALSE;
}
