xfs_buf_init(void)
{
	xfs_buf_zone = kmem_zone_init_flags(sizeof(xfs_buf_t), "xfs_buf",
						KM_ZONE_HWALIGN, NULL);
	if (!xfs_buf_zone)
		goto out;

	xfslogd_workqueue = alloc_workqueue("xfslogd",
					WQ_MEM_RECLAIM | WQ_HIGHPRI, 1);
	if (!xfslogd_workqueue)
		goto out_free_buf_zone;

	return 0;

 out_free_buf_zone:
	kmem_zone_destroy(xfs_buf_zone);
 out:
	return -ENOMEM;
}
