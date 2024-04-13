sdev_iter_datasets(struct vnode *dvp, int arg, char *name)
{
	zfs_cmd_t	*zc;
	int rc;

	sdcmn_err13(("iter name is '%s' (arg %x)", name, arg));
	zc = kmem_zalloc(sizeof (zfs_cmd_t), KM_SLEEP);
	(void) strcpy(zc->zc_name, name);

	while ((rc = devzvol_handle_ioctl(arg, zc, B_FALSE)) == 0) {
		struct vnode *vpp;
		char *ptr;

		sdcmn_err13(("  name %s", zc->zc_name));
		if (strchr(zc->zc_name, '$') || strchr(zc->zc_name, '%'))
			goto skip;
		ptr = strrchr(zc->zc_name, '/') + 1;
		rc = devzvol_lookup(dvp, ptr, &vpp, NULL, 0, NULL,
		    kcred, NULL, NULL, NULL);
		if (rc == 0) {
			VN_RELE(vpp);
		} else if (rc == ENOENT) {
			goto skip;
		} else {
			/*
			 * EBUSY == problem with zvols's dmu holds?
			 * EPERM when in a NGZ and traversing up and out.
			 */
			goto skip;
		}
		if (arg == ZFS_IOC_DATASET_LIST_NEXT &&
		    zc->zc_objset_stats.dds_type != DMU_OST_ZFS)
			sdev_iter_snapshots(dvp, zc->zc_name);
skip:
		(void) strcpy(zc->zc_name, name);
	}
	kmem_free(zc, sizeof (zfs_cmd_t));
}
