char *xmlrpc_double(char *buf, double value)
{
	*buf = '\0';
	snprintf(buf, XMLRPC_BUFSIZE, "<double>%g</double>", value);
	return buf;
}
