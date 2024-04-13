__xml_node_clean(xmlNode *xml) 
{
    xmlNode *cIter = NULL;
    xml_private_t *p = xml->_private;

    if(p) {
        p->flags = 0;
    }

    for (cIter = __xml_first_child(xml); cIter != NULL; cIter = __xml_next(cIter)) {
        __xml_node_clean(cIter);
    }
}
