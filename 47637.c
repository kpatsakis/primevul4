dump_filtered_xml(xmlNode * data, int options, char **buffer, int *offset, int *max)
{
    int lpc;
    xmlAttrPtr xIter = NULL;
    static int filter_len = DIMOF(filter);

    for (lpc = 0; options && lpc < filter_len; lpc++) {
        filter[lpc].found = FALSE;
    }

    for (xIter = crm_first_attr(data); xIter != NULL; xIter = xIter->next) {
        bool skip = FALSE;
        const char *p_name = (const char *)xIter->name;

        for (lpc = 0; skip == FALSE && lpc < filter_len; lpc++) {
            if (filter[lpc].found == FALSE && strcmp(p_name, filter[lpc].string) == 0) {
                filter[lpc].found = TRUE;
                skip = TRUE;
                break;
            }
        }

        if (skip == FALSE) {
            dump_xml_attr(xIter, options, buffer, offset, max);
        }
    }
}
