PJ_DEF(pj_xml_node*) pj_xml_node_new(pj_pool_t *pool, const pj_str_t *name)
{
    pj_xml_node *node = alloc_node(pool);
    pj_strdup(pool, &node->name, name);
    return node;
}