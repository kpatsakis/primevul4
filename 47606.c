add_node_nocopy(xmlNode * parent, const char *name, xmlNode * child)
{
    add_node_copy(parent, child);
    free_xml(child);
    return 1;
}
