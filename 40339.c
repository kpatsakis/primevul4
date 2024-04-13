int fb_get_options(char *name, char **option)
{
	char *opt, *options = NULL;
	int retval = 0;
	int name_len = strlen(name), i;

	if (name_len && ofonly && strncmp(name, "offb", 4))
		retval = 1;

	if (name_len && !retval) {
		for (i = 0; i < FB_MAX; i++) {
			if (video_options[i] == NULL)
				continue;
			if (!video_options[i][0])
				continue;
			opt = video_options[i];
			if (!strncmp(name, opt, name_len) &&
			    opt[name_len] == ':')
				options = opt + name_len + 1;
		}
	}
	if (options && !strncmp(options, "off", 3))
		retval = 1;

	if (option)
		*option = options;

	return retval;
}
