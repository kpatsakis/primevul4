__xml_acl_to_text(enum xml_private_flags flags)
{
    if(is_set(flags, xpf_acl_deny)) {
        return "deny";
    }
    if(is_set(flags, xpf_acl_write)) {
        return "read/write";
    }
    if(is_set(flags, xpf_acl_read)) {
        return "read";
    }

    return "none";
}
