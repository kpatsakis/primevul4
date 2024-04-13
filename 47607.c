add_xml_comment(xmlNode * parent, xmlNode * target, xmlNode * update)
{
    CRM_CHECK(update != NULL, return 0);
    CRM_CHECK(update->type == XML_COMMENT_NODE, return 0);

    if (target == NULL) {
        target = find_xml_comment(parent, update);
    } 
    
    if (target == NULL) {
        add_node_copy(parent, update);

    /* We wont reach here currently */
    } else if (safe_str_neq((const char *)target->content, (const char *)update->content)) {
        xmlFree(target->content);
        target->content = xmlStrdup(update->content);
    }

    return 0;
}
