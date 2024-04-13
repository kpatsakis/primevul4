static char *php_gethostbyname(char *name)
{
	struct hostent *hp;
	struct in_addr in;

	hp = gethostbyname(name);

	if (!hp || !*(hp->h_addr_list)) {
		return estrdup(name);
	}

	memcpy(&in.s_addr, *(hp->h_addr_list), sizeof(in.s_addr));

	return estrdup(inet_ntoa(in));
}
