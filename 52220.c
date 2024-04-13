bid_entry(const char *p, ssize_t len, ssize_t nl, int *last_is_path)
{
	int f = 0;
	static const unsigned char safe_char[256] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 00 - 0F */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 10 - 1F */
		/* !"$%&'()*+,-./  EXCLUSION:( )(#) */
		0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 20 - 2F */
		/* 0123456789:;<>?  EXCLUSION:(=) */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, /* 30 - 3F */
		/* @ABCDEFGHIJKLMNO */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 40 - 4F */
		/* PQRSTUVWXYZ[\]^_  */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 50 - 5F */
		/* `abcdefghijklmno */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 60 - 6F */
		/* pqrstuvwxyz{|}~ */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, /* 70 - 7F */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 80 - 8F */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 90 - 9F */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* A0 - AF */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* B0 - BF */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* C0 - CF */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* D0 - DF */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* E0 - EF */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* F0 - FF */
	};
	ssize_t ll;
	const char *pp = p;
	const char * const pp_end = pp + len;

	*last_is_path = 0;
	/*
	 * Skip the path-name which is quoted.
	 */
	for (;pp < pp_end; ++pp) {
		if (!safe_char[*(const unsigned char *)pp]) {
			if (*pp != ' ' && *pp != '\t' && *pp != '\r'
			    && *pp != '\n')
				f = 0;
			break;
		}
		f = 1;
	}
	ll = pp_end - pp;

	/* If a path-name was not found at the first, try to check
	 * a mtree format(a.k.a form D) ``NetBSD's mtree -D'' creates,
	 * which places the path-name at the last. */
	if (f == 0) {
		const char *pb = p + len - nl;
		int name_len = 0;
		int slash;

		/* The form D accepts only a single line for an entry. */
		if (pb-2 >= p &&
		    pb[-1] == '\\' && (pb[-2] == ' ' || pb[-2] == '\t'))
			return (-1);
		if (pb-1 >= p && pb[-1] == '\\')
			return (-1);

		slash = 0;
		while (p <= --pb && *pb != ' ' && *pb != '\t') {
			if (!safe_char[*(const unsigned char *)pb])
				return (-1);
			name_len++;
			/* The pathname should have a slash in this
			 * format. */
			if (*pb == '/')
				slash = 1;
		}
		if (name_len == 0 || slash == 0)
			return (-1);
		/* If '/' is placed at the first in this field, this is not
		 * a valid filename. */
		if (pb[1] == '/')
			return (-1);
		ll = len - nl - name_len;
		pp = p;
		*last_is_path = 1;
	}

	return (bid_keyword_list(pp, ll, 0, *last_is_path));
}
