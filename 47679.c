strip_text_nodes(xmlNode * xml)
{
    xmlNode *iter = xml->children;

    while (iter) {
        xmlNode *next = iter->next;

        switch (iter->type) {
            case XML_TEXT_NODE:
                /* Remove it */
                xmlUnlinkNode(iter);
                xmlFreeNode(iter);
                break;

            case XML_ELEMENT_NODE:
                /* Search it */
                strip_text_nodes(iter);
                break;

            default:
                /* Leave it */
                break;
        }

        iter = next;
    }
}
