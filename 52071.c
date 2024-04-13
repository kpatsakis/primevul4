void xmlrpc_send_string(const char *value)
{
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

	ss = " <param>\r\n  <value>\r\n   <string>";
	s->append(s, ss, strlen(ss));
	xmlrpc_append_char_encode(s, value);
	ss = "</string>\r\n  </value>\r\n </param>\r\n";
	s->append(s, ss, strlen(ss));

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
