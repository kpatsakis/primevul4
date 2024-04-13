options_for_tty()
{
    char *dev, *path, *p;
    int ret;
    size_t pl;

    dev = devnam;
    if ((p = strstr(dev, "/dev/")) != NULL)
	dev = p + 5;
    if (dev[0] == 0 || strcmp(dev, "tty") == 0)
	return 1;		/* don't look for /etc/ppp/options.tty */
    pl = strlen(_PATH_TTYOPT) + strlen(dev) + 1;
    path = malloc(pl);
    if (path == NULL)
	novm("tty init file name");
    slprintf(path, pl, "%s%s", _PATH_TTYOPT, dev);
    /* Turn slashes into dots, for Solaris case (e.g. /dev/term/a) */
    for (p = path + strlen(_PATH_TTYOPT); *p != 0; ++p)
	if (*p == '/')
	    *p = '.';
    option_priority = OPRIO_CFGFILE;
    ret = options_from_file(path, 0, 0, 1);
    free(path);
    return ret;
}
