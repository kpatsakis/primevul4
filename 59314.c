static char *get_eow(char *s, char *e)
{
	while (s<e && *s && !isblank(*s) && *s != '\n')
		s++;
	return s;
}
