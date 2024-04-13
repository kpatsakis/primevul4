xml_create_patchset_v2(xmlNode *source, xmlNode *target)
{
    int lpc = 0;
    GListPtr gIter = NULL;
    xml_private_t *doc = NULL;

    xmlNode *v = NULL;
    xmlNode *version = NULL;
    xmlNode *patchset = NULL;
    const char *vfields[] = {
        XML_ATTR_GENERATION_ADMIN,
        XML_ATTR_GENERATION,
        XML_ATTR_NUMUPDATES,
    };

    CRM_ASSERT(target);
    if(xml_document_dirty(target) == FALSE) {
        return NULL;
    }

    CRM_ASSERT(target->doc);
    doc = target->doc->_private;

    patchset = create_xml_node(NULL, XML_TAG_DIFF);
    crm_xml_add_int(patchset, "format", 2);

    version = create_xml_node(patchset, XML_DIFF_VERSION);

    v = create_xml_node(version, XML_DIFF_VSOURCE);
    for(lpc = 0; lpc < DIMOF(vfields); lpc++){
        const char *value = crm_element_value(source, vfields[lpc]);

        if(value == NULL) {
            value = "1";
        }
        crm_xml_add(v, vfields[lpc], value);
    }

    v = create_xml_node(version, XML_DIFF_VTARGET);
    for(lpc = 0; lpc < DIMOF(vfields); lpc++){
        const char *value = crm_element_value(target, vfields[lpc]);

        if(value == NULL) {
            value = "1";
        }
        crm_xml_add(v, vfields[lpc], value);
    }

    for(gIter = doc->deleted_paths; gIter; gIter = gIter->next) {
        xmlNode *change = create_xml_node(patchset, XML_DIFF_CHANGE);

        crm_xml_add(change, XML_DIFF_OP, "delete");
        crm_xml_add(change, XML_DIFF_PATH, gIter->data);
    }

    __xml_build_changes(target, patchset);
    return patchset;
}
