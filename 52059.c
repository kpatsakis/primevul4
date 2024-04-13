char *xmlrpc_boolean(char *buf, int value)
{
	*buf = '\0';
	snprintf(buf, XMLRPC_BUFSIZE, "<boolean>%d</boolean>", (value ? 1 : 0));
	return buf;
}
