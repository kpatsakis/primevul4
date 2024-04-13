xml_track_changes(xmlNode * xml, const char *user, xmlNode *acl_source, bool enforce_acls) 
{
    xml_accept_changes(xml);
    crm_trace("Tracking changes%s to %p", enforce_acls?" with ACLs":"", xml);
    set_doc_flag(xml, xpf_tracking);
    if(enforce_acls) {
        if(acl_source == NULL) {
            acl_source = xml;
        }
        set_doc_flag(xml, xpf_acl_enabled);
        __xml_acl_unpack(acl_source, xml, user);
        __xml_acl_apply(xml);
    }
}
