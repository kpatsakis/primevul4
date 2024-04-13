static char *xmlrpc_parse(char *buffer)
{
	char *tmp = NULL;

	/*
	   Okay since the buffer could contain
	   HTTP header information, lets break
	   off at the point that the <?xml?> starts
	 */
	tmp = strstr(buffer, "<?xml");

	/* check if its xml doc */
	if (tmp)
	{
		/* get all the odd characters out of the data */
		return xmlrpc_normalizeBuffer(tmp);
	}
	return NULL;
}
