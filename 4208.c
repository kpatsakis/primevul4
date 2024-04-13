PJ_DEF(pj_xml_node*) pj_xml_find_node_rec(const pj_xml_node *parent, 
					  const pj_str_t *name)
{
    const pj_xml_node *node = parent->node_head.next;

    PJ_CHECK_STACK();

    while (node != (void*)&parent->node_head) {
	pj_xml_node *found;
	if (pj_stricmp(&node->name, name) == 0)
	    return (pj_xml_node*)node;
	found = pj_xml_find_node_rec(node, name);
	if (found)
	    return (pj_xml_node*)found;
	node = node->next;
    }
    return NULL;
}