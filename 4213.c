PJ_DEF(pj_xml_attr*) pj_xml_find_attr( const pj_xml_node *node, 
				       const pj_str_t *name,
				       const pj_str_t *value)
{
    const pj_xml_attr *attr = node->attr_head.next;
    while (attr != (void*)&node->attr_head) {
	if (pj_stricmp(&attr->name, name)==0) {
	    if (value) {
		if (pj_stricmp(&attr->value, value)==0)
		    return (pj_xml_attr*)attr;
	    } else {
		return (pj_xml_attr*)attr;
	    }
	}
	attr = attr->next;
    }
    return NULL;
}