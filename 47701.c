bool xml_document_dirty(xmlNode *xml) 
{
    if(xml != NULL && xml->doc && xml->doc->_private) {
        xml_private_t *doc = xml->doc->_private;

        return is_set(doc->flags, xpf_dirty);
    }
    return FALSE;
}
