static XMLRPCCmd *createXMLCommand(const char *name, XMLRPCMethodFunc func)
{
	XMLRPCCmd *xml = NULL;
	xml = smalloc(sizeof(XMLRPCCmd));
	xml->name = sstrdup(name);
	xml->func = func;
	xml->mod_name = NULL;
	xml->core = 0;
	xml->next = NULL;
	return xml;
}
