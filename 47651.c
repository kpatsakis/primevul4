first_named_child(xmlNode * parent, const char *name)
{
    xmlNode *match = NULL;

    for (match = __xml_first_child(parent); match != NULL; match = __xml_next(match)) {
        /*
         * name == NULL gives first child regardless of name; this is
         * semantically incorrect in this funciton, but may be necessary
         * due to prior use of xml_child_iter_filter
         */
        if (name == NULL || strcmp((const char *)match->name, name) == 0) {
            return match;
        }
    }
    return NULL;
}
