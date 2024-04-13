__xml_acl_parse_entry(xmlNode * acl_top, xmlNode * acl_entry, xmlNode *target)
{
    xmlNode *child = NULL;

    for (child = __xml_first_child(acl_entry); child; child = __xml_next(child)) {
        const char *tag = crm_element_name(child);
        const char *kind = crm_element_value(child, XML_ACL_ATTR_KIND);

        if (strcmp(XML_ACL_TAG_PERMISSION, tag) == 0){
            tag = kind;
        }

        crm_trace("Processing %s %p", tag, child);
        if(tag == NULL) {
            CRM_ASSERT(tag != NULL);

        } else if (strcmp(XML_ACL_TAG_ROLE_REF, tag) == 0
                   || strcmp(XML_ACL_TAG_ROLE_REFv1, tag) == 0) {
            const char *ref_role = crm_element_value(child, XML_ATTR_ID);

            if (ref_role) {
                xmlNode *role = NULL;

                for (role = __xml_first_child(acl_top); role; role = __xml_next(role)) {
                    if (strcmp(XML_ACL_TAG_ROLE, (const char *)role->name) == 0) {
                        const char *role_id = crm_element_value(role, XML_ATTR_ID);

                        if (role_id && strcmp(ref_role, role_id) == 0) {
                            crm_debug("Unpacking referenced role: %s", role_id);
                            __xml_acl_parse_entry(acl_top, role, target);
                            break;
                        }
                    }
                }
            }

        } else if (strcmp(XML_ACL_TAG_READ, tag) == 0) {
            __xml_acl_create(child, target, xpf_acl_read);

        } else if (strcmp(XML_ACL_TAG_WRITE, tag) == 0) {
            __xml_acl_create(child, target, xpf_acl_write);

        } else if (strcmp(XML_ACL_TAG_DENY, tag) == 0) {
            __xml_acl_create(child, target, xpf_acl_deny);

        } else {
            crm_warn("Unknown ACL entry: %s/%s", tag, kind);
        }
    }

    return TRUE;
}
