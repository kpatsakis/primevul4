devzvol_create_pool_dirs(struct vnode *dvp)
{
	zfs_cmd_t	*zc;
	nvlist_t *nv = NULL;
	nvpair_t *elem = NULL;
	size_t size;
	int pools = 0;
	int rc;

	sdcmn_err13(("devzvol_create_pool_dirs"));
	zc = kmem_zalloc(sizeof (zfs_cmd_t), KM_SLEEP);
	mutex_enter(&devzvol_mtx);
	zc->zc_cookie = devzvol_gen;

	rc = devzvol_handle_ioctl(ZFS_IOC_POOL_CONFIGS, zc, &size);
	switch (rc) {
		case 0:
			/* new generation */
			ASSERT(devzvol_gen != zc->zc_cookie);
			devzvol_gen = zc->zc_cookie;
			if (devzvol_zclist)
				kmem_free((void *)(uintptr_t)devzvol_zclist,
				    devzvol_zclist_size);
			devzvol_zclist = zc->zc_nvlist_dst;
			devzvol_zclist_size = size;
			break;
		case EEXIST:
			/*
			 * no change in the configuration; still need
			 * to do lookups in case we did a lookup in
			 * zvol/rdsk but not zvol/dsk (or vice versa)
			 */
			kmem_free((void *)(uintptr_t)zc->zc_nvlist_dst,
			    size);
			break;
		default:
			kmem_free((void *)(uintptr_t)zc->zc_nvlist_dst,
			    size);
			goto out;
	}
	rc = nvlist_unpack((char *)(uintptr_t)devzvol_zclist,
	    devzvol_zclist_size, &nv, 0);
	if (rc) {
		ASSERT(rc == 0);
		kmem_free((void *)(uintptr_t)devzvol_zclist,
		    devzvol_zclist_size);
		devzvol_gen = 0;
		devzvol_zclist = NULL;
		devzvol_zclist_size = 0;
		goto out;
	}
	mutex_exit(&devzvol_mtx);
	while ((elem = nvlist_next_nvpair(nv, elem)) != NULL) {
		struct vnode *vp;
		ASSERT(dvp->v_count > 0);
		rc = VOP_LOOKUP(dvp, nvpair_name(elem), &vp, NULL, 0,
		    NULL, kcred, NULL, 0, NULL);
		/* should either work, or not be visible from a zone */
		ASSERT(rc == 0 || rc == ENOENT);
		if (rc == 0)
			VN_RELE(vp);
		pools++;
	}
	nvlist_free(nv);
	mutex_enter(&devzvol_mtx);
	if (devzvol_isopen && pools == 0) {
		/* clean up so zfs can be unloaded */
		devzvol_close_zfs();
		devzvol_isopen = B_FALSE;
	}
out:
	mutex_exit(&devzvol_mtx);
	kmem_free(zc, sizeof (zfs_cmd_t));
}
