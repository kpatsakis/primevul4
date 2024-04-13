static void gluster_cache_refresh(glfs_t *fs, const char *cfgstring)
{
	struct gluster_cacheconn **entry;
	char** config;
	size_t i = 0;
	size_t j = 0;

	if (!fs)
		return;

	darray_foreach(entry, cache) {
		if ((*entry)->fs == fs) {
			if (cfgstring) {
				darray_foreach(config, (*entry)->cfgstring) {
					if (!strcmp(*config, cfgstring)) {
						free(*config);
						darray_remove((*entry)->cfgstring, j);
						break;
					}
					j++;
				}
			}

			if (darray_size((*entry)->cfgstring))
				return;

			free((*entry)->volname);
			glfs_fini((*entry)->fs);
			(*entry)->fs = NULL;
			gluster_free_host((*entry)->server);
			free((*entry)->server);
			(*entry)->server = NULL;
			free((*entry));

			darray_remove(cache, i);
			return;
		} else {
			i++;
		}
	}
}
