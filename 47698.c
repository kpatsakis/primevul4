xml_calculate_changes(xmlNode * old, xmlNode * new)
{
    CRM_CHECK(safe_str_eq(crm_element_name(old), crm_element_name(new)), return);
    CRM_CHECK(safe_str_eq(ID(old), ID(new)), return);

    if(xml_tracking_changes(new) == FALSE) {
        xml_track_changes(new, NULL, NULL, FALSE);
    }

    __xml_diff_object(old, new);
}
