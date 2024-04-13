loadplugin(argv)
    char **argv;
{
    char *arg = *argv;
    void *handle;
    const char *err;
    void (*init) __P((void));
    char *path = arg;
    const char *vers;

    if (strchr(arg, '/') == 0) {
	const char *base = _PATH_PLUGIN;
	int l = strlen(base) + strlen(arg) + 2;
	path = malloc(l);
	if (path == 0)
	    novm("plugin file path");
	strlcpy(path, base, l);
	strlcat(path, "/", l);
	strlcat(path, arg, l);
    }
    handle = dlopen(path, RTLD_GLOBAL | RTLD_NOW);
    if (handle == 0) {
	err = dlerror();
	if (err != 0)
	    option_error("%s", err);
	option_error("Couldn't load plugin %s", arg);
	goto err;
    }
    init = (void (*)(void))dlsym(handle, "plugin_init");
    if (init == 0) {
	option_error("%s has no initialization entry point", arg);
	goto errclose;
    }
    vers = (const char *) dlsym(handle, "pppd_version");
    if (vers == 0) {
	warn("Warning: plugin %s has no version information", arg);
    } else if (strcmp(vers, VERSION) != 0) {
	option_error("Plugin %s is for pppd version %s, this is %s",
		     arg, vers, VERSION);
	goto errclose;
    }
    info("Plugin %s loaded.", arg);
    (*init)();
    return 1;

 errclose:
    dlclose(handle);
 err:
    if (path != arg)
	free(path);
    return 0;
}
