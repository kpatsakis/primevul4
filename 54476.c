static void tcmu_glfs_close(struct tcmu_device *dev)
{
	struct glfs_state *gfsp = tcmu_get_dev_private(dev);

	glfs_close(gfsp->gfd);
	gluster_cache_refresh(gfsp->fs, tcmu_get_path(dev));
	gluster_free_server(&gfsp->hosts);
	free(gfsp);
}
