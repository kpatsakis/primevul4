xml_log_changes(uint8_t log_level, const char *function, xmlNode * xml)
{
    GListPtr gIter = NULL;
    xml_private_t *doc = NULL;

    CRM_ASSERT(xml);
    CRM_ASSERT(xml->doc);

    doc = xml->doc->_private;
    if(is_not_set(doc->flags, xpf_dirty)) {
        return;
    }

    for(gIter = doc->deleted_paths; gIter; gIter = gIter->next) {
        do_crm_log_alias(log_level, __FILE__, function, __LINE__, "-- %s", (char*)gIter->data);
    }

    log_data_element(log_level, __FILE__, function, __LINE__, "+ ", xml, 0,
                     xml_log_option_formatted|xml_log_option_dirty_add);
}
