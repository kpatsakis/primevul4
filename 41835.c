xfs_buf_terminate(void)
{
	destroy_workqueue(xfslogd_workqueue);
	kmem_zone_destroy(xfs_buf_zone);
}
