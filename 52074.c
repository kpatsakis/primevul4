static int xmlrpc_split_buf(char *buffer, char ***argv)
{
	int ac = 0;
	int argvsize = 8;
	char *data, *str;
	char *nexttag = NULL;
	char *p;
	int tagtype = 0;

	data = buffer;
	*argv = smalloc(sizeof(char *) * argvsize);
	while ((data = strstr(data, "<value>")))
	{
		data += 7;
		nexttag = strchr(data, '<');
		if (nexttag == NULL)
			break;
		nexttag++;
		p = strchr(nexttag, '>');
		if (p == NULL)
			break;
		*p++ = '\0';
		/* strings */
		if (!stricmp("string", nexttag))
			tagtype = 1;
		else
			tagtype = 0;
		str = p;
		p = strchr(str, '<');
		if (p == NULL)
			break;
		*p++ = '\0';
		if (ac >= argvsize)
		{
			argvsize *= 2;
			*argv = srealloc(*argv, sizeof(char *) * argvsize);
		}
		if (tagtype == 1)
			(*argv)[ac++] = xmlrpc_decode_string(str);
		else
			(*argv)[ac++] = str;
		data = p;
	}			/* while */
	return ac;
}
