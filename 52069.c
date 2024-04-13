int xmlrpc_register_method(const char *name, XMLRPCMethodFunc func)
{
	XMLRPCCmd *xml;

	return_val_if_fail(name != NULL, XMLRPC_ERR_PARAMS);
	return_val_if_fail(func != NULL, XMLRPC_ERR_PARAMS);
	xml = createXMLCommand(name, func);

	return addXMLCommand(xml);
}
