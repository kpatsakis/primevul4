static size_t log_prefix(const char *p, unsigned int *level, char *special)
{
	unsigned int lev = 0;
	char sp = '\0';
	size_t len;

	if (p[0] != '<' || !p[1])
		return 0;
	if (p[2] == '>') {
		/* usual single digit level number or special char */
		switch (p[1]) {
		case '0' ... '7':
			lev = p[1] - '0';
			break;
		case 'c': /* KERN_CONT */
		case 'd': /* KERN_DEFAULT */
			sp = p[1];
			break;
		default:
			return 0;
		}
		len = 3;
	} else {
		/* multi digit including the level and facility number */
		char *endp = NULL;

		lev = (simple_strtoul(&p[1], &endp, 10) & 7);
		if (endp == NULL || endp[0] != '>')
			return 0;
		len = (endp + 1) - p;
	}

	/* do not accept special char if not asked for */
	if (sp && !special)
		return 0;

	if (special) {
		*special = sp;
		/* return special char, do not touch level */
		if (sp)
			return len;
	}

	if (level)
		*level = lev;
	return len;
}
