is_config_change(xmlNode *xml)
{
    GListPtr gIter = NULL;
    xml_private_t *p = NULL;
    xmlNode *config = first_named_child(xml, XML_CIB_TAG_CONFIGURATION);

    if(config) {
        p = config->_private;
    }
    if(p && is_set(p->flags, xpf_dirty)) {
        return TRUE;
    }

    if(xml->doc && xml->doc->_private) {
        p = xml->doc->_private;
        for(gIter = p->deleted_paths; gIter; gIter = gIter->next) {
            char *path = gIter->data;

            if(strstr(path, "/"XML_TAG_CIB"/"XML_CIB_TAG_CONFIGURATION) != NULL) {
                return TRUE;
            }
        }
    }

    return FALSE;
}
