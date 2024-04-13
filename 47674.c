set_parent_flag(xmlNode *xml, long flag) 
{

    for(; xml; xml = xml->parent) {
        xml_private_t *p = xml->_private;

        if(p == NULL) {
            /* During calls to xmlDocCopyNode(), _private will be unset for parent nodes */
        } else {
            p->flags |= flag;
            /* crm_trace("Setting flag %x due to %s[@id=%s]", flag, xml->name, ID(xml)); */
        }
    }
}
