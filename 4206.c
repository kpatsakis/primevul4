PJ_DEF(pj_xml_node*) pj_xml_clone( pj_pool_t *pool, const pj_xml_node *rhs)
{
    pj_xml_node *node;
    const pj_xml_attr *r_attr;
    const pj_xml_node *child;

    node = alloc_node(pool);

    pj_strdup(pool, &node->name, &rhs->name);
    pj_strdup(pool, &node->content, &rhs->content);

    /* Clone all attributes */
    r_attr = rhs->attr_head.next;
    while (r_attr != &rhs->attr_head) {

	pj_xml_attr *attr;

	attr = alloc_attr(pool);
	pj_strdup(pool, &attr->name, &r_attr->name);
	pj_strdup(pool, &attr->value, &r_attr->value);

	pj_list_push_back(&node->attr_head, attr);

	r_attr = r_attr->next;
    }

    /* Clone all child nodes. */
    child = rhs->node_head.next;
    while (child != (pj_xml_node*) &rhs->node_head) {
	pj_xml_node *new_child;

	new_child = pj_xml_clone(pool, child);
	pj_list_push_back(&node->node_head, new_child);

	child = child->next;
    }

    return node;
}