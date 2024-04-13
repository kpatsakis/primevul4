static int __init video_setup(char *options)
{
	int i, global = 0;

	if (!options || !*options)
 		global = 1;

 	if (!global && !strncmp(options, "ofonly", 6)) {
 		ofonly = 1;
 		global = 1;
 	}

 	if (!global && !strchr(options, ':')) {
 		fb_mode_option = options;
 		global = 1;
 	}

 	if (!global) {
 		for (i = 0; i < FB_MAX; i++) {
 			if (video_options[i] == NULL) {
 				video_options[i] = options;
 				break;
 			}

		}
	}

	return 1;
}
