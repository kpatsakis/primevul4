void xmlrpc_send(int argc, ...)
{
	va_list va;
	int idx = 0;
	int len;
	char buf[1024];
	const char *ss;
	mowgli_string_t *s = mowgli_string_create();
	char *s2;
	char *header;

	if (xmlrpc.encode)
	{
		snprintf(buf, sizeof buf, "<?xml version=\"1.0\" encoding=\"%s\" ?>\r\n<methodResponse>\r\n<params>\r\n", xmlrpc.encode);
	}
	else
	{
		snprintf(buf, sizeof buf, "<?xml version=\"1.0\"?>\r\n<methodResponse>\r\n<params>\r\n");
	}
	s->append(s, buf, strlen(buf));

	va_start(va, argc);
	for (idx = 0; idx < argc; idx++)
	{
		ss = " <param>\r\n  <value>\r\n   ";
		s->append(s, ss, strlen(ss));
		ss = va_arg(va, const char *);
		s->append(s, ss, strlen(ss));
		ss = "\r\n  </value>\r\n </param>\r\n";
		s->append(s, ss, strlen(ss));
	}
	va_end(va);

	ss = "</params>\r\n</methodResponse>";
	s->append(s, ss, strlen(ss));

	len = s->pos;

	if (xmlrpc.httpheader)
	{
		header = xmlrpc_write_header(len);
		s2 = smalloc(strlen(header) + len + 1);
		strcpy(s2, header);
		memcpy(s2 + strlen(header), s->str, len);
		xmlrpc.setbuffer(s2, len + strlen(header));
		free(header);
		free(s2);
		xmlrpc.httpheader = 1;
	}
	else
	{
		xmlrpc.setbuffer(s->str, len);
	}
	if (xmlrpc.encode)
	{
		free(xmlrpc.encode);
		xmlrpc.encode = NULL;
	}

	s->destroy(s);
}
