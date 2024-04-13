static glfs_t* gluster_cache_query(gluster_server *dst, char *cfgstring)
{
	struct gluster_cacheconn **entry;
	char** config;
	char* cfg_copy = NULL;
	bool cfgmatch = false;

	darray_foreach(entry, cache) {
		if (strcmp((*entry)->volname, dst->volname))
			continue;
		if (gluster_compare_hosts((*entry)->server, dst->server)) {

			darray_foreach(config, (*entry)->cfgstring) {
				if (!strcmp(*config, cfgstring)) {
					cfgmatch = true;
					break;
				}
			}
			if (!cfgmatch) {
				cfg_copy = strdup(cfgstring);
				darray_append((*entry)->cfgstring, cfg_copy);
			}
			return (*entry)->fs;
		}
	}

	return NULL;
}
