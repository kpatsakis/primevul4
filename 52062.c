void xmlrpc_generic_error(int code, const char *string)
{
	char buf[1024];
	const char *ss;
	mowgli_string_t *s = mowgli_string_create();
	char *s2;
	int len;

	if (xmlrpc.encode)
	{
		snprintf(buf, sizeof buf, "<?xml version=\"1.0\" encoding=\"%s\" ?>\r\n<methodResponse>\r\n", xmlrpc.encode);
	}
	else
	{
		snprintf(buf, sizeof buf, "<?xml version=\"1.0\"?>\r\n<methodResponse>\r\n");
	}
	s->append(s, buf, strlen(buf));

	ss = " <fault>\r\n  <value>\r\n   <struct>\r\n    <member>\r\n     <name>faultCode</name>\r\n     <value><int>";
	s->append(s, ss, strlen(ss));
	snprintf(buf, sizeof buf, "%d", code);
	s->append(s, buf, strlen(buf));
	ss = "</int></value>\r\n    </member>\r\n    <member>\r\n     <name>faultString</name>\r\n     <value><string>";
	s->append(s, ss, strlen(ss));
	xmlrpc_append_char_encode(s, string);
	ss = "</string></value>\r\n    </member>\r\n   </struct>\r\n  </value>\r\n </fault>\r\n</methodResponse>",
	s->append(s, ss, strlen(ss));

	len = s->pos;

	if (xmlrpc.httpheader)
	{
		char *header = xmlrpc_write_header(len);
		s2 = smalloc(strlen(header) + len + 1);
		strcpy(s2, header);
		memcpy(s2 + strlen(header), s->str, len);
		xmlrpc.setbuffer(s2, len + strlen(header));
		free(header);
		free(s2);
	}
	else
		xmlrpc.setbuffer(s->str, len);

	s->destroy(s);
}
