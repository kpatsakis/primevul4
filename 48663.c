devzvol_lookup(struct vnode *dvp, char *nm, struct vnode **vpp,
    struct pathname *pnp, int flags, struct vnode *rdir, struct cred *cred,
    caller_context_t *ct, int *direntflags, pathname_t *realpnp)
{
	enum vtype expected_type = VDIR;
	struct sdev_node *parent = VTOSDEV(dvp);
	char *dsname;
	dmu_objset_type_t do_type;
	int error;

	sdcmn_err13(("devzvol_lookup '%s' '%s'", parent->sdev_path, nm));
	*vpp = NULL;
	/* execute access is required to search the directory */
	if ((error = VOP_ACCESS(dvp, VEXEC, 0, cred, ct)) != 0)
		return (error);

	rw_enter(&parent->sdev_contents, RW_READER);
	if (SDEV_IS_GLOBAL(parent)) {
		/*
		 * During iter_datasets, don't create GZ dev when running in
		 * NGZ.  We can't return ENOENT here since that could
		 * incorrectly trigger the creation of the dev from the
		 * recursive call through prof_filldir during iter_datasets.
		 */
		if (getzoneid() != GLOBAL_ZONEID) {
			rw_exit(&parent->sdev_contents);
			return (EPERM);
		}
	} else {
		int res;

		rw_exit(&parent->sdev_contents);

		/*
		 * If we're in the global zone and reach down into a non-global
		 * zone's /dev/zvol then this action could trigger the creation
		 * of all of the zvol devices for every zone into the non-global
		 * zone's /dev tree. This could be a big security hole. To
		 * prevent this, disallow the global zone from looking inside
		 * a non-global zones /dev/zvol. This behavior is similar to
		 * delegated datasets, which cannot be used by the global zone.
		 */
		if (getzoneid() == GLOBAL_ZONEID)
			return (EPERM);

		res = prof_lookup(dvp, nm, vpp, cred);

		/*
		 * We won't find a zvol that was dynamically created inside
		 * a NGZ, within a delegated dataset, in the zone's dev profile
		 * but prof_lookup will also find it via sdev_cache_lookup.
		 */
		if (res == ENOENT) {
			/*
			 * We have to create the sdev node for the dymamically
			 * created zvol.
			 */
			if (devzvol_mk_ngz_node(parent, nm) != 0)
				return (ENOENT);
			res = prof_lookup(dvp, nm, vpp, cred);
		}

		return (res);
	}

	dsname = devzvol_make_dsname(parent->sdev_path, nm);
	rw_exit(&parent->sdev_contents);
	sdcmn_err13(("rvp dsname %s", dsname ? dsname : "(null)"));
	if (dsname) {
		error = devzvol_objset_check(dsname, &do_type);
		if (error != 0) {
			error = ENOENT;
			goto out;
		}
		if (do_type == DMU_OST_ZVOL)
			expected_type = VLNK;
	}
	/*
	 * the callbacks expect:
	 *
	 * parent->sdev_path		   nm
	 * /dev/zvol			   {r}dsk
	 * /dev/zvol/{r}dsk		   <pool name>
	 * /dev/zvol/{r}dsk/<dataset name> <last ds component>
	 *
	 * sdev_name is always last path component of sdev_path
	 */
	if (expected_type == VDIR) {
		error = devname_lookup_func(parent, nm, vpp, cred,
		    devzvol_create_dir, SDEV_VATTR);
	} else {
		error = devname_lookup_func(parent, nm, vpp, cred,
		    devzvol_create_link, SDEV_VLINK);
	}
	sdcmn_err13(("devzvol_lookup %d %d", expected_type, error));
	ASSERT(error || ((*vpp)->v_type == expected_type));
out:
	if (dsname)
		kmem_free(dsname, strlen(dsname) + 1);
	sdcmn_err13(("devzvol_lookup %d", error));
	return (error);
}
