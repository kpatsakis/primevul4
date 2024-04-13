devzvol_objset_check(char *dsname, dmu_objset_type_t *type)
{
	boolean_t	ispool;
	zfs_cmd_t	*zc;
	int rc;

	zc = kmem_zalloc(sizeof (zfs_cmd_t), KM_SLEEP);
	(void) strlcpy(zc->zc_name, dsname, MAXPATHLEN);

	ispool = (strchr(dsname, '/') == NULL) ? B_TRUE : B_FALSE;
	if (!ispool && sdev_zvol_name2minor(dsname, NULL) == 0) {
		sdcmn_err13(("found cached minor node"));
		if (type)
			*type = DMU_OST_ZVOL;
		kmem_free(zc, sizeof (zfs_cmd_t));
		return (0);
	}
	rc = devzvol_handle_ioctl(ispool ? ZFS_IOC_POOL_STATS :
	    ZFS_IOC_OBJSET_STATS, zc, NULL);
	if (type && rc == 0)
		*type = (ispool) ? DMU_OST_ZFS :
		    zc->zc_objset_stats.dds_type;
	kmem_free(zc, sizeof (zfs_cmd_t));
	return (rc);
}
