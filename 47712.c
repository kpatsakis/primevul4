bool xml_tracking_changes(xmlNode * xml)
{
    if(xml == NULL) {
        return FALSE;

    } else if(is_set(((xml_private_t *)xml->doc->_private)->flags, xpf_tracking)) {
        return TRUE;
    }
    return FALSE;
}
