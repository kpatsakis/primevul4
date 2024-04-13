char *xmlrpc_array(int argc, ...)
{
	va_list va;
	char *a;
	int idx = 0;
	char *s = NULL;
	int len;
	char buf[XMLRPC_BUFSIZE];

	va_start(va, argc);
	for (idx = 0; idx < argc; idx++)
	{
		a = va_arg(va, char *);
		if (!s)
		{
			snprintf(buf, XMLRPC_BUFSIZE, "   <value>%s</value>", a);
			s = sstrdup(buf);
		}
		else
		{
			snprintf(buf, XMLRPC_BUFSIZE, "%s\r\n     <value>%s</value>", s, a);
			free(s);
			s = sstrdup(buf);
		}
	}
	va_end(va);

	snprintf(buf, XMLRPC_BUFSIZE, "<array>\r\n    <data>\r\n  %s\r\n    </data>\r\n   </array>", s);
	len = strlen(buf);
	free(s);
	return sstrdup(buf);
}
