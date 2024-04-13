add_node_copy(xmlNode * parent, xmlNode * src_node)
{
    xmlNode *child = NULL;
    xmlDoc *doc = getDocPtr(parent);

    CRM_CHECK(src_node != NULL, return NULL);

    child = xmlDocCopyNode(src_node, doc, 1);
    xmlAddChild(parent, child);
    crm_node_created(child);
    return child;
}
