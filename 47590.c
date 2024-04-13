__xml_acl_unpack(xmlNode *source, xmlNode *target, const char *user)
{
#if ENABLE_ACL
    xml_private_t *p = NULL;

    if(target == NULL || target->doc == NULL || target->doc->_private == NULL) {
        return;
    }

    p = target->doc->_private;
    if(pcmk_acl_required(user) == FALSE) {
        crm_trace("no acls needed for '%s'", user);

    } else if(p->acls == NULL) {
        xmlNode *acls = get_xpath_object("//"XML_CIB_TAG_ACLS, source, LOG_TRACE);

        free(p->user);
        p->user = strdup(user);

        if(acls) {
            xmlNode *child = NULL;

            for (child = __xml_first_child(acls); child; child = __xml_next(child)) {
                const char *tag = crm_element_name(child);

                if (strcmp(tag, XML_ACL_TAG_USER) == 0 || strcmp(tag, XML_ACL_TAG_USERv1) == 0) {
                    const char *id = crm_element_value(child, XML_ATTR_ID);

                    if(id && strcmp(id, user) == 0) {
                        crm_debug("Unpacking ACLs for %s", id);
                        __xml_acl_parse_entry(acls, child, target);
                    }
                }
            }
        }
    }
#endif
}
