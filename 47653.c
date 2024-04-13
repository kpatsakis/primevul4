free_xml(xmlNode * child)
{
    if (child != NULL) {
        xmlNode *top = NULL;
        xmlDoc *doc = child->doc;
        xml_private_t *p = child->_private;

        if (doc != NULL) {
            top = xmlDocGetRootElement(doc);
        }

        if (doc != NULL && top == child) {
            /* Free everything */
            xmlFreeDoc(doc);

        } else if(__xml_acl_check(child, NULL, xpf_acl_write) == FALSE) {
            int offset = 0;
            char buffer[XML_BUFFER_SIZE];

            __get_prefix(NULL, child, buffer, offset);
            crm_trace("Cannot remove %s %x", buffer, p->flags);
            return;

        } else {
            if(doc && TRACKING_CHANGES(child) && is_not_set(p->flags, xpf_created)) {
                int offset = 0;
                char buffer[XML_BUFFER_SIZE];

                if(__get_prefix(NULL, child, buffer, offset) > 0) {
                    crm_trace("Deleting %s %p from %p", buffer, child, doc);
                    p = doc->_private;
                    p->deleted_paths = g_list_append(p->deleted_paths, strdup(buffer));
                    set_doc_flag(child, xpf_dirty);
                }
            }

            /* Free this particular subtree
             * Make sure to unlink it from the parent first
             */
            xmlUnlinkNode(child);
            xmlFreeNode(child);
        }
    }
}
