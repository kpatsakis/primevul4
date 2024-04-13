bool xml_patch_versions(xmlNode *patchset, int add[3], int del[3])
{
    int lpc = 0;
    int format = 1;
    xmlNode *tmp = NULL;

    const char *vfields[] = {
        XML_ATTR_GENERATION_ADMIN,
        XML_ATTR_GENERATION,
        XML_ATTR_NUMUPDATES,
    };


    crm_element_value_int(patchset, "format", &format);
    switch(format) {
        case 1:
            tmp = find_xml_node(patchset, "diff-removed", FALSE);
            tmp = find_xml_node(tmp, "cib", FALSE);
            if(tmp == NULL) {
                /* Revert to the diff-removed line */
                tmp = find_xml_node(patchset, "diff-removed", FALSE);
            }
            break;
        case 2:
            tmp = find_xml_node(patchset, "version", FALSE);
            tmp = find_xml_node(tmp, "source", FALSE);
            break;
        default:
            crm_warn("Unknown patch format: %d", format);
            return -EINVAL;
    }

    if (tmp) {
        for(lpc = 0; lpc < DIMOF(vfields); lpc++) {
            crm_element_value_int(tmp, vfields[lpc], &(del[lpc]));
            crm_trace("Got %d for del[%s]", del[lpc], vfields[lpc]);
        }
    }

    switch(format) {
        case 1:
            tmp = find_xml_node(patchset, "diff-added", FALSE);
            tmp = find_xml_node(tmp, "cib", FALSE);
            if(tmp == NULL) {
                /* Revert to the diff-added line */
                tmp = find_xml_node(patchset, "diff-added", FALSE);
            }
            break;
        case 2:
            tmp = find_xml_node(patchset, "version", FALSE);
            tmp = find_xml_node(tmp, "target", FALSE);
            break;
        default:
            crm_warn("Unknown patch format: %d", format);
            return -EINVAL;
    }

    if (tmp) {
        for(lpc = 0; lpc < DIMOF(vfields); lpc++) {
            crm_element_value_int(tmp, vfields[lpc], &(add[lpc]));
            crm_trace("Got %d for add[%s]", add[lpc], vfields[lpc]);
        }
    }

    return pcmk_ok;
}
