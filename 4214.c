PJ_DEF(void) pj_xml_add_node( pj_xml_node *parent, pj_xml_node *node )
{
    pj_list_push_back(&parent->node_head, node);
}