static char *find_line(char *p, char *e, char *u, char *t, char *l)
{
	char *p1, *p2, *ret;

	while (p<e  && (p1 = get_eol(p, e)) < e) {
		ret = p;
		if (*p == '#')
			goto next;
		while (p<e && isblank(*p)) p++;
		p2 = get_eow(p, e);
		if (!p2 || p2-p != strlen(u) || strncmp(p, u, strlen(u)) != 0)
			goto next;
		p = p2+1;
		while (p<e && isblank(*p)) p++;
		p2 = get_eow(p, e);
		if (!p2 || p2-p != strlen(t) || strncmp(p, t, strlen(t)) != 0)
			goto next;
		p = p2+1;
		while (p<e && isblank(*p)) p++;
		p2 = get_eow(p, e);
		if (!p2 || p2-p != strlen(l) || strncmp(p, l, strlen(l)) != 0)
			goto next;
		return ret;
next:
		p = p1 + 1;
	}

	return NULL;
}
