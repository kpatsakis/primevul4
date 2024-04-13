static char *xmlrpc_method(char *buffer)
{
	char *data, *p, *name;
	int namelen;

	data = strstr(buffer, "<methodName>");
	if (data)
	{
		data += 12;
		p = strchr(data, '<');
		if (p == NULL)
			return NULL;
		namelen = p - data;
		name = smalloc(namelen + 1);
		memcpy(name, data, namelen);
		name[namelen] = '\0';
		return name;
	}
	return NULL;
}
