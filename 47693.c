xml_acl_enabled(xmlNode *xml)
{
    if(xml && xml->doc && xml->doc->_private){
        xml_private_t *p = xml->doc->_private;

        return is_set(p->flags, xpf_acl_enabled);
    }
    return FALSE;
}
