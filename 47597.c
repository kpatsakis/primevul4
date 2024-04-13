__xml_node_dirty(xmlNode *xml) 
{
    set_doc_flag(xml, xpf_dirty);
    set_parent_flag(xml, xpf_dirty);
}
