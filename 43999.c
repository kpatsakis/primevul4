callfile(argv)
    char **argv;
{
    char *fname, *arg, *p;
    int l, ok;

    arg = *argv;
    ok = 1;
    if (arg[0] == '/' || arg[0] == 0)
	ok = 0;
    else {
	for (p = arg; *p != 0; ) {
	    if (p[0] == '.' && p[1] == '.' && (p[2] == '/' || p[2] == 0)) {
		ok = 0;
		break;
	    }
	    while (*p != '/' && *p != 0)
		++p;
	    if (*p == '/')
		++p;
	}
    }
    if (!ok) {
	option_error("call option value may not contain .. or start with /");
	return 0;
    }

    l = strlen(arg) + strlen(_PATH_PEERFILES) + 1;
    if ((fname = (char *) malloc(l)) == NULL)
	novm("call file name");
    slprintf(fname, l, "%s%s", _PATH_PEERFILES, arg);

    ok = options_from_file(fname, 1, 1, 1);

    free(fname);
    return ok;
}
