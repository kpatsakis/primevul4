crm_node_created(xmlNode *xml) 
{
    xmlNode *cIter = NULL;
    xml_private_t *p = xml->_private;

    if(p && TRACKING_CHANGES(xml)) {
        if(is_not_set(p->flags, xpf_created)) {
            p->flags |= xpf_created;
            __xml_node_dirty(xml);
        }

        for (cIter = __xml_first_child(xml); cIter != NULL; cIter = __xml_next(cIter)) {
           crm_node_created(cIter);
        }
    }
}
