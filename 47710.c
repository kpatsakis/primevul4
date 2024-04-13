xml_remove_prop(xmlNode * obj, const char *name)
{
    if(__xml_acl_check(obj, NULL, xpf_acl_write) == FALSE) {
        crm_trace("Cannot remove %s from %s", name, obj->name);

    } else if(TRACKING_CHANGES(obj)) {
        /* Leave in place (marked for removal) until after the diff is calculated */
        xml_private_t *p = NULL;
        xmlAttr *attr = xmlHasProp(obj, (const xmlChar *)name);

        p = attr->_private;
        set_parent_flag(obj, xpf_dirty);
        p->flags |= xpf_deleted;
        /* crm_trace("Setting flag %x due to %s[@id=%s].%s", xpf_dirty, obj->name, ID(obj), name); */

    } else {
        xmlUnsetProp(obj, (const xmlChar *)name);
    }
}
