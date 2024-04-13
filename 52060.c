char *xmlrpc_decode_string(char *buf)
{
	const char *p;
	char *q;

	p = buf;
	q = buf;
	while (*p != '\0')
	{
		if (*p == '&')
		{
			p++;
			if (!strncmp(p, "gt;", 3))
				*q++ = '>', p += 3;
			else if (!strncmp(p, "lt;", 3))
				*q++ = '<', p += 3;
			else if (!strncmp(p, "quot;", 5))
				*q++ = '"', p += 5;
			else if (!strncmp(p, "amp;", 4))
				*q++ = '&', p += 4;
			else if (*p == '#')
			{
				p++;
				*q++ = (char)atoi(p);
				while (*p != ';' && *p != '\0')
					p++;
			}
		}
		else
			*q++ = *p++;
	}
	*q = '\0';

	return buf;
}
