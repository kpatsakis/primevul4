pcmkDeregisterNode(xmlNodePtr node)
{
    __xml_private_free(node->_private);
}
