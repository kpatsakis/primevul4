char *xmlrpc_integer(char *buf, int value)
{
	*buf = '\0';

	if (!xmlrpc.inttagstart || !xmlrpc.inttagend)
	{
		snprintf(buf, XMLRPC_BUFSIZE, "<i4>%d</i4>", value);
	}
	else
	{
		snprintf(buf, XMLRPC_BUFSIZE, "%s%d%s", xmlrpc.inttagstart, value, xmlrpc.inttagend);
		free(xmlrpc.inttagstart);
		if (xmlrpc.inttagend)
		{
			free(xmlrpc.inttagend);
			xmlrpc.inttagend = NULL;
		}
		xmlrpc.inttagstart = NULL;
	}
	return buf;
}
