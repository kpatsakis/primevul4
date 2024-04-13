la_strsep(char **sp, const char *sep)
{
	char *p, *s;
	if (sp == NULL || *sp == NULL || **sp == '\0')
		return(NULL);
	s = *sp;
	p = s + strcspn(s, sep);
	if (*p != '\0')
		*p++ = '\0';
	*sp = p;
	return(s);
}
