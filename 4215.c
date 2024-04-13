static int xml_print_node( const pj_xml_node *node, int indent, 
			   char *buf, pj_size_t len )
{
    int i;
    char *p = buf;
    pj_xml_attr *attr;
    pj_xml_node *sub_node;

#define SIZE_LEFT()	((int)(len - (p-buf)))

    PJ_CHECK_STACK();

    /* Print name. */
    if (SIZE_LEFT() < node->name.slen + indent + 5)
	return -1;
    for (i=0; i<indent; ++i)
	*p++ = ' ';
    *p++ = '<';
    pj_memcpy(p, node->name.ptr, node->name.slen);
    p += node->name.slen;

    /* Print attributes. */
    attr = node->attr_head.next;
    while (attr != &node->attr_head) {

	if (SIZE_LEFT() < attr->name.slen + attr->value.slen + 4)
	    return -1;

	*p++ = ' ';

	/* Attribute name. */
	pj_memcpy(p, attr->name.ptr, attr->name.slen);
	p += attr->name.slen;

	/* Attribute value. */
	if (attr->value.slen) {
	    *p++ = '=';
	    *p++ = '"';
	    pj_memcpy(p, attr->value.ptr, attr->value.slen);
	    p += attr->value.slen;
	    *p++ = '"';
	}

	attr = attr->next;
    }

    /* Check for empty node. */
    if (node->content.slen==0 &&
	node->node_head.next==(pj_xml_node*)&node->node_head)
    {
        if (SIZE_LEFT() < 3) return -1;
	*p++ = ' ';
	*p++ = '/';
	*p++ = '>';
	return (int)(p-buf);
    }

    /* Enclosing '>' */
    if (SIZE_LEFT() < 1) return -1;
    *p++ = '>';

    /* Print sub nodes. */
    sub_node = node->node_head.next;
    while (sub_node != (pj_xml_node*)&node->node_head) {
	int printed;

	if (SIZE_LEFT() < indent + 3)
	    return -1;
	//*p++ = '\r';
	*p++ = '\n';

	printed = xml_print_node(sub_node, indent + 1, p, SIZE_LEFT());
	if (printed < 0)
	    return -1;

	p += printed;
	sub_node = sub_node->next;
    }

    /* Content. */
    if (node->content.slen) {
	if (SIZE_LEFT() < node->content.slen) return -1;
	pj_memcpy(p, node->content.ptr, node->content.slen);
	p += node->content.slen;
    }

    /* Enclosing node. */
    if (node->node_head.next != (pj_xml_node*)&node->node_head) {
	if (SIZE_LEFT() < node->name.slen + 5 + indent)
	    return -1;
	//*p++ = '\r';
	*p++ = '\n';
	for (i=0; i<indent; ++i)
	    *p++ = ' ';
    } else {
	if (SIZE_LEFT() < node->name.slen + 3)
	    return -1;
    }
    *p++ = '<';
    *p++ = '/';
    pj_memcpy(p, node->name.ptr, node->name.slen);
    p += node->name.slen;
    *p++ = '>';

#undef SIZE_LEFT

    return (int)(p-buf);
}