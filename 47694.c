xml_acl_filtered_copy(const char *user, xmlNode* acl_source, xmlNode *xml, xmlNode ** result)
{
    GListPtr aIter = NULL;
    xmlNode *target = NULL;
    xml_private_t *p = NULL;
    xml_private_t *doc = NULL;

    *result = NULL;
    if(xml == NULL || pcmk_acl_required(user) == FALSE) {
        crm_trace("no acls needed for '%s'", user);
        return FALSE;
    }

    crm_trace("filtering copy of %p for '%s'", xml, user);
    target = copy_xml(xml);
    if(target == NULL) {
        return TRUE;
    }

    __xml_acl_unpack(acl_source, target, user);
    set_doc_flag(target, xpf_acl_enabled);
    __xml_acl_apply(target);

    doc = target->doc->_private;
    for(aIter = doc->acls; aIter != NULL && target; aIter = aIter->next) {
        int max = 0;
        xml_acl_t *acl = aIter->data;

        if(acl->mode != xpf_acl_deny) {
            /* Nothing to do */

        } else if(acl->xpath) {
            int lpc = 0;
            xmlXPathObjectPtr xpathObj = xpath_search(target, acl->xpath);

            max = numXpathResults(xpathObj);
            for(lpc = 0; lpc < max; lpc++) {
                xmlNode *match = getXpathResult(xpathObj, lpc);

                crm_trace("Purging attributes from %s", acl->xpath);
                if(__xml_purge_attributes(match) == FALSE && match == target) {
                    crm_trace("No access to the entire document for %s", user);
                    freeXpathObject(xpathObj);
                    return TRUE;
                }
            }
            crm_trace("Enforced ACL %s (%d matches)", acl->xpath, max);
            freeXpathObject(xpathObj);
        }
    }

    p = target->_private;
    if(is_set(p->flags, xpf_acl_deny) && __xml_purge_attributes(target) == FALSE) {
        crm_trace("No access to the entire document for %s", user);
        return TRUE;
    }

    if(doc->acls) {
        g_list_free_full(doc->acls, __xml_acl_free);
        doc->acls = NULL;

    } else {
        crm_trace("Ordinary user '%s' cannot access the CIB without any defined ACLs", doc->user);
        free_xml(target);
        target = NULL;
    }

    if(target) {
        *result = target;
    }

    return TRUE;
}
