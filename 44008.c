options_from_file(filename, must_exist, check_prot, priv)
    char *filename;
    int must_exist;
    int check_prot;
    int priv;
{
    FILE *f;
    int i, newline, ret, err;
    option_t *opt;
    int oldpriv, n;
    char *oldsource;
    uid_t euid;
    char *argv[MAXARGS];
    char args[MAXARGS][MAXWORDLEN];
    char cmd[MAXWORDLEN];

    euid = geteuid();
    if (check_prot && seteuid(getuid()) == -1) {
	option_error("unable to drop privileges to open %s: %m", filename);
	return 0;
    }
    f = fopen(filename, "r");
    err = errno;
    if (check_prot && seteuid(euid) == -1)
	fatal("unable to regain privileges");
    if (f == NULL) {
	errno = err;
	if (!must_exist) {
	    if (err != ENOENT && err != ENOTDIR)
		warn("Warning: can't open options file %s: %m", filename);
	    return 1;
	}
	option_error("Can't open options file %s: %m", filename);
	return 0;
    }

    oldpriv = privileged_option;
    privileged_option = priv;
    oldsource = option_source;
    option_source = strdup(filename);
    if (option_source == NULL)
	option_source = "file";
    ret = 0;
    while (getword(f, cmd, &newline, filename)) {
	opt = find_option(cmd);
	if (opt == NULL) {
	    option_error("In file %s: unrecognized option '%s'",
			 filename, cmd);
	    goto err;
	}
	n = n_arguments(opt);
	for (i = 0; i < n; ++i) {
	    if (!getword(f, args[i], &newline, filename)) {
		option_error(
			"In file %s: too few parameters for option '%s'",
			filename, cmd);
		goto err;
	    }
	    argv[i] = args[i];
	}
	if (!process_option(opt, cmd, argv))
	    goto err;
    }
    ret = 1;

err:
    fclose(f);
    privileged_option = oldpriv;
    option_source = oldsource;
    return ret;
}
