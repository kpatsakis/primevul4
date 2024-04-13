char *xmlrpc_string(char *buf, const char *value)
{
	char encoded[XMLRPC_BUFSIZE];
	*buf = '\0';

	xmlrpc_char_encode(encoded, value);
	snprintf(buf, XMLRPC_BUFSIZE, "<string>%s</string>", encoded);
	return buf;
}
