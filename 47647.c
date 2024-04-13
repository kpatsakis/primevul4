find_entity(xmlNode * parent, const char *node_name, const char *id)
{
    xmlNode *a_child = NULL;

    for (a_child = __xml_first_child(parent); a_child != NULL; a_child = __xml_next(a_child)) {
        /* Uncertain if node_name == NULL check is strictly necessary here */
        if (node_name == NULL || strcmp((const char *)a_child->name, node_name) == 0) {
            const char *cid = ID(a_child);
            if (id == NULL || (cid != NULL && strcmp(id, cid) == 0)) {
                return a_child;
            }
        }
    }

    crm_trace("node <%s id=%s> not found in %s.", node_name, id, crm_element_name(parent));
    return NULL;
}
