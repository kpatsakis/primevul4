PJ_DEF(pj_xml_node*) pj_xml_find( const pj_xml_node *parent, 
				  const pj_str_t *name,
				  const void *data, 
				  pj_bool_t (*match)(const pj_xml_node *, 
						     const void*))
{
    const pj_xml_node *node = (const pj_xml_node *)parent->node_head.next;

    if (!name && !match)
	return NULL;

    while (node != (const pj_xml_node*) &parent->node_head) {
	if (name) {
	    if (pj_stricmp(&node->name, name)!=0) {
		node = node->next;
		continue;
	    }
	}
	if (match) {
	    if (match(node, data))
		return (pj_xml_node*)node;
	} else {
	    return (pj_xml_node*)node;
	}

	node = node->next;
    }
    return NULL;
}