int xmlrpc_set_options(int type, const char *value)
{
	if (type == XMLRPC_HTTP_HEADER)
	{
		if (!stricmp(value, XMLRPC_ON))
		{
			xmlrpc.httpheader = 1;
		}
		if (!stricmp(value, XMLRPC_OFF))
		{
			xmlrpc.httpheader = 0;
		}
	}
	if (type == XMLRPC_ENCODE)
	{
		if (value)
		{
			xmlrpc.encode = sstrdup(value);
		}
	}
	if (type == XMLRPC_INTTAG)
	{
		if (!stricmp(value, XMLRPC_I4))
		{
			xmlrpc.inttagstart = sstrdup("<i4>");
			xmlrpc.inttagend = sstrdup("</i4>");
		}
		if (!stricmp(value, XMLRPC_INT))
		{
			xmlrpc.inttagstart = sstrdup("<int>");
			xmlrpc.inttagend = sstrdup("</int>");
		}
	}
	return 1;
}
