static glfs_t* tcmu_create_glfs_object(char *config, gluster_server **hosts)
{
	gluster_server *entry = NULL;
	char logfilepath[PATH_MAX];
    glfs_t *fs =  NULL;
    int ret = -1;

	if (parse_imagepath(config, hosts) == -1) {
		tcmu_err("hostaddr, volname, or path missing\n");
		goto fail;
	}
	entry = *hosts;

	fs = gluster_cache_query(entry, config);
	if (fs)
		return fs;

	fs = glfs_new(entry->volname);
	if (!fs) {
		tcmu_err("glfs_new failed\n");
		goto fail;
	}

	ret = gluster_cache_add(entry, fs, config);
	if (ret) {
		tcmu_err("gluster_cache_add failed: %m\n");
		goto fail;
	}

	ret = glfs_set_volfile_server(fs,
				gluster_transport_lookup[entry->server->type],
				entry->server->u.inet.addr,
				atoi(entry->server->u.inet.port));
	if (ret) {
		tcmu_err("glfs_set_volfile_server failed: %m\n");
		goto unref;
	}

	ret = tcmu_make_absolute_logfile(logfilepath, TCMU_GLFS_LOG_FILENAME);
	if (ret < 0) {
		tcmu_err("tcmu_make_absolute_logfile failed: %m\n");
		goto unref;
	}

	ret = glfs_set_logging(fs, logfilepath, TCMU_GLFS_DEBUG_LEVEL);
	if (ret < 0) {
		tcmu_err("glfs_set_logging failed: %m\n");
		goto unref;
	}

	ret = glfs_init(fs);
	if (ret) {
		tcmu_err("glfs_init failed: %m\n");
		goto unref;
	}

	return fs;

 unref:
	gluster_cache_refresh(fs, config);

 fail:
	gluster_free_server(hosts);
	return NULL;
}
