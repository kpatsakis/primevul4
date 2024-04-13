parsehex(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'a' && c <= 'f')
		return c - 'a';
	else if (c >= 'A' && c <= 'F')
		return c - 'A';
	else
		return -1;
}
