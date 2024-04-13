static int gluster_cache_add(gluster_server *dst, glfs_t *fs, char* cfgstring)
{
	struct gluster_cacheconn *entry;
	char* cfg_copy = NULL;

	entry = calloc(1, sizeof(gluster_cacheconn));
	if (!entry)
		goto error;

	entry->volname = strdup(dst->volname);

	entry->server = calloc(1, sizeof(gluster_hostdef));
	if (!entry->server)
		goto error;

	entry->server->type = dst->server->type;

	if (entry->server->type == GLUSTER_TRANSPORT_UNIX) {
		entry->server->u.uds.socket = strdup(dst->server->u.uds.socket);
	} else {
		entry->server->u.inet.addr = strdup(dst->server->u.inet.addr);
		entry->server->u.inet.port = strdup(dst->server->u.inet.port);
	}

	entry->fs = fs;

	cfg_copy = strdup(cfgstring);
	darray_init(entry->cfgstring);
	darray_append(entry->cfgstring, cfg_copy);

	darray_append(cache, entry);

	return 0;

 error:
	return -1;
}
