static pj_xml_node *alloc_node( pj_pool_t *pool )
{
    pj_xml_node *node;

    node = PJ_POOL_ZALLOC_T(pool, pj_xml_node);
    pj_list_init( &node->attr_head );
    pj_list_init( &node->node_head );

    return node;
}