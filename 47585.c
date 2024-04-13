__xml_acl_create(xmlNode * xml, xmlNode *target, enum xml_private_flags mode)
{
    xml_acl_t *acl = NULL;

    xml_private_t *p = NULL;
    const char *tag = crm_element_value(xml, XML_ACL_ATTR_TAG);
    const char *ref = crm_element_value(xml, XML_ACL_ATTR_REF);
    const char *xpath = crm_element_value(xml, XML_ACL_ATTR_XPATH);

    if(tag == NULL) {
        /* Compatability handling for pacemaker < 1.1.12 */
        tag = crm_element_value(xml, XML_ACL_ATTR_TAGv1);
    }
    if(ref == NULL) {
        /* Compatability handling for pacemaker < 1.1.12 */
        ref = crm_element_value(xml, XML_ACL_ATTR_REFv1);
    }

    if(target == NULL || target->doc == NULL || target->doc->_private == NULL){
        CRM_ASSERT(target);
        CRM_ASSERT(target->doc);
        CRM_ASSERT(target->doc->_private);
        return NULL;

    } else if (tag == NULL && ref == NULL && xpath == NULL) {
        crm_trace("No criteria %p", xml);
        return NULL;
    }

    p = target->doc->_private;
    acl = calloc(1, sizeof(xml_acl_t));
    if (acl) {
        const char *attr = crm_element_value(xml, XML_ACL_ATTR_ATTRIBUTE);

        acl->mode = mode;
        if(xpath) {
            acl->xpath = strdup(xpath);
            crm_trace("Using xpath: %s", acl->xpath);

        } else {
            int offset = 0;
            char buffer[XML_BUFFER_SIZE];

            if(tag) {
                offset += snprintf(buffer + offset, XML_BUFFER_SIZE - offset, "//%s", tag);
            } else {
                offset += snprintf(buffer + offset, XML_BUFFER_SIZE - offset, "//*");
            }

            if(ref || attr) {
                offset += snprintf(buffer + offset, XML_BUFFER_SIZE - offset, "[");
            }

            if(ref) {
                offset += snprintf(buffer + offset, XML_BUFFER_SIZE - offset, "@id='%s'", ref);
            }

            if(ref && attr) {
                offset += snprintf(buffer + offset, XML_BUFFER_SIZE - offset, " and ");
            }

            if(attr) {
                offset += snprintf(buffer + offset, XML_BUFFER_SIZE - offset, "@%s", attr);
            }

            if(ref || attr) {
                offset += snprintf(buffer + offset, XML_BUFFER_SIZE - offset, "]");
            }

            CRM_LOG_ASSERT(offset > 0);
            acl->xpath = strdup(buffer);
            crm_trace("Built xpath: %s", acl->xpath);
        }

        p->acls = g_list_append(p->acls, acl);
    }
    return acl;
}
