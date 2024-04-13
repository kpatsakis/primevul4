xml_create_patchset_v1(xmlNode *source, xmlNode *target, bool config, bool with_digest)
{
    xmlNode *patchset = diff_xml_object(source, target, !with_digest);

    if(patchset) {
        CRM_LOG_ASSERT(xml_document_dirty(target));
        xml_repair_v1_diff(source, target, patchset, config);
        crm_xml_add(patchset, "format", "1");
    }
    return patchset;
}
