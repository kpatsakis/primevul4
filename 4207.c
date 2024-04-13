PJ_DEF(void) pj_xml_add_attr( pj_xml_node *node, pj_xml_attr *attr )
{
    pj_list_push_back(&node->attr_head, attr);
}