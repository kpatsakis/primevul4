devzvol_mk_ngz_node(struct sdev_node *parent, char *nm)
{
	struct vattr vattr;
	timestruc_t now;
	enum vtype expected_type = VDIR;
	dmu_objset_type_t do_type;
	struct sdev_node *dv = NULL;
	int res;
	char *dsname;

	bzero(&vattr, sizeof (vattr));
	gethrestime(&now);
	vattr.va_mask = AT_TYPE|AT_MODE|AT_UID|AT_GID;
	vattr.va_uid = SDEV_UID_DEFAULT;
	vattr.va_gid = SDEV_GID_DEFAULT;
	vattr.va_type = VNON;
	vattr.va_atime = now;
	vattr.va_mtime = now;
	vattr.va_ctime = now;

	if ((dsname = devzvol_make_dsname(parent->sdev_path, nm)) == NULL)
		return (ENOENT);

	if (devzvol_objset_check(dsname, &do_type) != 0) {
		kmem_free(dsname, strlen(dsname) + 1);
		return (ENOENT);
	}
	if (do_type == DMU_OST_ZVOL)
		expected_type = VBLK;

	if (expected_type == VDIR) {
		vattr.va_type = VDIR;
		vattr.va_mode = SDEV_DIRMODE_DEFAULT;
	} else {
		minor_t minor;
		dev_t devnum;
		int rc;

		rc = sdev_zvol_create_minor(dsname);
		if ((rc != 0 && rc != EEXIST && rc != EBUSY) ||
		    sdev_zvol_name2minor(dsname, &minor)) {
			kmem_free(dsname, strlen(dsname) + 1);
			return (ENOENT);
		}

		devnum = makedevice(devzvol_major, minor);
		vattr.va_rdev = devnum;

		if (strstr(parent->sdev_path, "/rdsk/") != NULL)
			vattr.va_type = VCHR;
		else
			vattr.va_type = VBLK;
		vattr.va_mode = SDEV_DEVMODE_DEFAULT;
	}
	kmem_free(dsname, strlen(dsname) + 1);

	rw_enter(&parent->sdev_contents, RW_WRITER);

	res = sdev_mknode(parent, nm, &dv, &vattr,
	    NULL, NULL, kcred, SDEV_READY);
	rw_exit(&parent->sdev_contents);
	if (res != 0)
		return (ENOENT);

	SDEV_RELE(dv);
	return (0);
}
