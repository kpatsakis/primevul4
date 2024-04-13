static int __init xen_blkif_init(void)
{
	int rc = 0;

	if (!xen_domain())
		return -ENODEV;

	if (xen_blkif_max_ring_order > XENBUS_MAX_RING_GRANT_ORDER) {
		pr_info("Invalid max_ring_order (%d), will use default max: %d.\n",
			xen_blkif_max_ring_order, XENBUS_MAX_RING_GRANT_ORDER);
		xen_blkif_max_ring_order = XENBUS_MAX_RING_GRANT_ORDER;
	}

	if (xenblk_max_queues == 0)
		xenblk_max_queues = num_online_cpus();

	rc = xen_blkif_interface_init();
	if (rc)
		goto failed_init;

	rc = xen_blkif_xenbus_init();
	if (rc)
		goto failed_init;

 failed_init:
	return rc;
}
