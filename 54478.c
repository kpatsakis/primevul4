 static int tcmu_glfs_open(struct tcmu_device *dev)
 {
 	struct glfs_state *gfsp;
	int ret = 0;
	char *config;
	struct stat st;

	gfsp = calloc(1, sizeof(*gfsp));
	if (!gfsp)
		return -ENOMEM;

	tcmu_set_dev_private(dev, gfsp);

	config = tcmu_get_path(dev);
	if (!config) {
		goto fail;
	}

	gfsp->fs = tcmu_create_glfs_object(config, &gfsp->hosts);
	if (!gfsp->fs) {
		tcmu_err("tcmu_create_glfs_object failed\n");
		goto fail;
	}

	gfsp->gfd = glfs_open(gfsp->fs, gfsp->hosts->path, ALLOWED_BSOFLAGS);
	if (!gfsp->gfd) {
		tcmu_err("glfs_open failed: %m\n");
		goto unref;
	}

	ret = glfs_lstat(gfsp->fs, gfsp->hosts->path, &st);
	if (ret) {
		tcmu_err("glfs_lstat failed: %m\n");
		goto unref;
	}

	if (st.st_size != tcmu_get_device_size(dev)) {
		tcmu_err("device size and backing size disagree: "
		       "device %lld backing %lld\n",
		       tcmu_get_device_size(dev),
		       (long long) st.st_size);
		goto unref;
	}

	return 0;

unref:
	gluster_cache_refresh(gfsp->fs, tcmu_get_path(dev));

fail:
	if (gfsp->gfd)
		glfs_close(gfsp->gfd);
	gluster_free_server(&gfsp->hosts);
	free(gfsp);

	return -EIO;
}
