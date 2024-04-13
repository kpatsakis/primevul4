devzvol_handle_ioctl(int cmd, zfs_cmd_t *zc, size_t *alloc_size)
{
	uint64_t cookie;
	int size = 8000;
	int unused;
	int rc;

	if (cmd != ZFS_IOC_POOL_CONFIGS)
		mutex_enter(&devzvol_mtx);
	if (!devzvol_isopen) {
		if ((rc = devzvol_open_zfs()) == 0) {
			devzvol_isopen = B_TRUE;
		} else {
			if (cmd != ZFS_IOC_POOL_CONFIGS)
				mutex_exit(&devzvol_mtx);
			return (ENXIO);
		}
	}
	cookie = zc->zc_cookie;
again:
	zc->zc_nvlist_dst = (uint64_t)(intptr_t)kmem_alloc(size,
	    KM_SLEEP);
	zc->zc_nvlist_dst_size = size;
	rc = ldi_ioctl(devzvol_lh, cmd, (intptr_t)zc, FKIOCTL, kcred,
	    &unused);
	if (rc == ENOMEM) {
		int newsize;
		newsize = zc->zc_nvlist_dst_size;
		ASSERT(newsize > size);
		kmem_free((void *)(uintptr_t)zc->zc_nvlist_dst, size);
		size = newsize;
		zc->zc_cookie = cookie;
		goto again;
	}
	if (alloc_size == NULL)
		kmem_free((void *)(uintptr_t)zc->zc_nvlist_dst, size);
	else
		*alloc_size = size;
	if (cmd != ZFS_IOC_POOL_CONFIGS)
		mutex_exit(&devzvol_mtx);
	return (rc);
}
