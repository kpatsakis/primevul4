__first_xml_child_match(xmlNode *parent, const char *name, const char *id)
{
    xmlNode *cIter = NULL;

    for (cIter = __xml_first_child(parent); cIter != NULL; cIter = __xml_next(cIter)) {
        if(strcmp((const char *)cIter->name, name) != 0) {
            continue;
        } else if(id) {
            const char *cid = ID(cIter);
            if(cid == NULL || strcmp(cid, id) != 0) {
                continue;
            }
        }
        return cIter;
    }
    return NULL;
}
