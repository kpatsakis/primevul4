find_xml_comment(xmlNode * root, xmlNode * search_comment)
{
    xmlNode *a_child = NULL;

    CRM_CHECK(search_comment->type == XML_COMMENT_NODE, return NULL);

    for (a_child = __xml_first_child(root); a_child != NULL; a_child = __xml_next(a_child)) {
        if (a_child->type != XML_COMMENT_NODE) {
            continue;
        }
        if (safe_str_eq((const char *)a_child->content, (const char *)search_comment->content)) {
            return a_child;
        }
    }

    return NULL;
}
