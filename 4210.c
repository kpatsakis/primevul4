PJ_DEF(pj_xml_node*) pj_xml_find_next_node( const pj_xml_node *parent, 
					    const pj_xml_node *node,
					    const pj_str_t *name)
{
    PJ_CHECK_STACK();

    node = node->next;
    while (node != (void*)&parent->node_head) {
	if (pj_stricmp(&node->name, name) == 0)
	    return (pj_xml_node*)node;
	node = node->next;
    }
    return NULL;
}