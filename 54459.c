static int open_handlers(void)
{
	struct dirent **dirent_list;
	int num_handlers;
	int num_good = 0;
	int i;

	num_handlers = scandir(handler_path, &dirent_list, is_handler, alphasort);

	if (num_handlers == -1)
		return -1;

	for (i = 0; i < num_handlers; i++) {
		char *path;
		void *handle;
		int (*handler_init)(void);
		int ret;

		ret = asprintf(&path, "%s/%s", handler_path, dirent_list[i]->d_name);
		if (ret == -1) {
			tcmu_err("ENOMEM\n");
			continue;
		}

		handle = dlopen(path, RTLD_NOW|RTLD_LOCAL);
		if (!handle) {
			tcmu_err("Could not open handler at %s: %s\n", path, dlerror());
			free(path);
			continue;
		}

		handler_init = dlsym(handle, "handler_init");
		if (!handler_init) {
			tcmu_err("dlsym failure on %s\n", path);
			free(path);
			continue;
		}

		ret = handler_init();

		free(path);

		if (ret == 0)
			num_good++;
	}

	for (i = 0; i < num_handlers; i++)
		free(dirent_list[i]);
	free(dirent_list);

	return num_good;
}
