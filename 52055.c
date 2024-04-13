static int addXMLCommand(XMLRPCCmd * xml)
{
	if (XMLRPCCMD == NULL)
		XMLRPCCMD = mowgli_patricia_create(strcasecanon);

	mowgli_patricia_add(XMLRPCCMD, xml->name, xml);

	return XMLRPC_ERR_OK;
}
