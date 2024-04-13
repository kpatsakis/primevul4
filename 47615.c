crm_attr_dirty(xmlAttr *a) 
{
    xmlNode *parent = a->parent;
    xml_private_t *p = NULL;

    p = a->_private;
    p->flags |= (xpf_dirty|xpf_modified);
    p->flags = (p->flags & ~xpf_deleted);
    /* crm_trace("Setting flag %x due to %s[@id=%s, @%s=%s]", */
    /*           xpf_dirty, parent?parent->name:NULL, ID(parent), a->name, a->children->content); */

    __xml_node_dirty(parent);
}
