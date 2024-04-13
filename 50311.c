Sprintf(char *fmt, ...)
{
    int len = 0;
    int status = SP_NORMAL;
    int p = 0;
    char *f;
    Str s;
    va_list ap;

    va_start(ap, fmt);
    for (f = fmt; *f; f++) {
      redo:
	switch (status) {
	case SP_NORMAL:
	    if (*f == '%') {
		status = SP_PREC;
		p = 0;
	    }
	    else
		len++;
	    break;
	case SP_PREC:
	    if (IS_ALPHA(*f)) {
		/* conversion char. */
		double vd;
		int vi;
		char *vs;
		void *vp;

		switch (*f) {
		case 'l':
		case 'h':
		case 'L':
		case 'w':
		    continue;
		case 'd':
		case 'i':
		case 'o':
		case 'x':
		case 'X':
		case 'u':
		    vi = va_arg(ap, int);
		    len += (p > 0) ? p : 10;
		    break;
		case 'f':
		case 'g':
		case 'e':
		case 'G':
		case 'E':
		    vd = va_arg(ap, double);
		    len += (p > 0) ? p : 15;
		    break;
		case 'c':
		    len += 1;
		    vi = va_arg(ap, int);
		    break;
		case 's':
		    vs = va_arg(ap, char *);
		    vi = strlen(vs);
		    len += (p > vi) ? p : vi;
		    break;
		case 'p':
		    vp = va_arg(ap, void *);
		    len += 10;
		    break;
		case 'n':
		    vp = va_arg(ap, void *);
		    break;
		}
		status = SP_NORMAL;
	    }
	    else if (IS_DIGIT(*f))
		p = p * 10 + *f - '0';
	    else if (*f == '.')
		status = SP_PREC2;
	    else if (*f == '%') {
		status = SP_NORMAL;
		len++;
	    }
	    break;
	case SP_PREC2:
	    if (IS_ALPHA(*f)) {
		status = SP_PREC;
		goto redo;
	    }
	    break;
	}
    }
    va_end(ap);
    s = Strnew_size(len * 2);
    va_start(ap, fmt);
    vsprintf(s->ptr, fmt, ap);
    va_end(ap);
    s->length = strlen(s->ptr);
    if (s->length > len * 2) {
	fprintf(stderr, "Sprintf: string too long\n");
	exit(1);
    }
    return s;
}
