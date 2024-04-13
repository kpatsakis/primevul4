unistr(netdissect_options *ndo,
       const u_char *s, uint32_t *len, int use_unicode)
{
    static char buf[MAX_UNISTR_SIZE+1];
    size_t l = 0;
    uint32_t strsize;
    const u_char *sp;

    if (use_unicode) {
	/*
	 * Skip padding that puts the string on an even boundary.
	 */
	if (((s - startbuf) % 2) != 0) {
	    ND_TCHECK(s[0]);
	    s++;
	}
    }
    if (*len == 0) {
	/*
	 * Null-terminated string.
	 */
	strsize = 0;
	sp = s;
	if (!use_unicode) {
	    for (;;) {
		ND_TCHECK(sp[0]);
		*len += 1;
		if (sp[0] == 0)
		    break;
		sp++;
	    }
	    strsize = *len - 1;
	} else {
	    for (;;) {
		ND_TCHECK2(sp[0], 2);
		*len += 2;
		if (sp[0] == 0 && sp[1] == 0)
		    break;
		sp += 2;
	    }
	    strsize = *len - 2;
	}
    } else {
	/*
	 * Counted string.
	 */
	strsize = *len;
    }
    if (!use_unicode) {
    	while (strsize != 0) {
          ND_TCHECK(s[0]);
	    if (l >= MAX_UNISTR_SIZE)
		break;
	    if (ND_ISPRINT(s[0]))
		buf[l] = s[0];
	    else {
		if (s[0] == 0)
		    break;
		buf[l] = '.';
	    }
	    l++;
	    s++;
	    strsize--;
	}
    } else {
	while (strsize != 0) {
	    ND_TCHECK2(s[0], 2);
	    if (l >= MAX_UNISTR_SIZE)
		break;
	    if (s[1] == 0 && ND_ISPRINT(s[0])) {
		/* It's a printable ASCII character */
		buf[l] = s[0];
	    } else {
		/* It's a non-ASCII character or a non-printable ASCII character */
		if (s[0] == 0 && s[1] == 0)
		    break;
		buf[l] = '.';
	    }
	    l++;
	    s += 2;
	    if (strsize == 1)
		break;
	    strsize -= 2;
	}
    }
    buf[l] = 0;
    return buf;

trunc:
    return NULL;
}
