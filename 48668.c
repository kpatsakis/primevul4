devzvol_validate(struct sdev_node *dv)
{
	dmu_objset_type_t do_type;
	char *dsname;
	char *nm = dv->sdev_name;
	int rc;

	sdcmn_err13(("validating ('%s' '%s')", dv->sdev_path, nm));
	/*
	 * validate only READY nodes; if someone is sitting on the
	 * directory of a dataset that just got destroyed we could
	 * get a zombie node which we just skip.
	 */
	if (dv->sdev_state != SDEV_READY) {
		sdcmn_err13(("skipping '%s'", nm));
		return (SDEV_VTOR_SKIP);
	}

	if ((strcmp(dv->sdev_path, ZVOL_DIR "/dsk") == 0) ||
	    (strcmp(dv->sdev_path, ZVOL_DIR "/rdsk") == 0))
		return (SDEV_VTOR_VALID);
	dsname = devzvol_make_dsname(dv->sdev_path, NULL);
	if (dsname == NULL)
		return (SDEV_VTOR_INVALID);

	rc = devzvol_objset_check(dsname, &do_type);
	sdcmn_err13(("  '%s' rc %d", dsname, rc));
	if (rc != 0) {
		kmem_free(dsname, strlen(dsname) + 1);
		return (SDEV_VTOR_INVALID);
	}
	sdcmn_err13(("  v_type %d do_type %d",
	    SDEVTOV(dv)->v_type, do_type));
	if ((SDEVTOV(dv)->v_type == VLNK && do_type != DMU_OST_ZVOL) ||
	    ((SDEVTOV(dv)->v_type == VBLK || SDEVTOV(dv)->v_type == VCHR) &&
	    do_type != DMU_OST_ZVOL) ||
	    (SDEVTOV(dv)->v_type == VDIR && do_type == DMU_OST_ZVOL)) {
		kmem_free(dsname, strlen(dsname) + 1);
		return (SDEV_VTOR_STALE);
	}
	if (SDEVTOV(dv)->v_type == VLNK) {
		char *ptr, *link;
		long val = 0;
		minor_t lminor, ominor;

		rc = sdev_getlink(SDEVTOV(dv), &link);
		ASSERT(rc == 0);

		ptr = strrchr(link, ':') + 1;
		rc = ddi_strtol(ptr, NULL, 10, &val);
		kmem_free(link, strlen(link) + 1);
		ASSERT(rc == 0 && val != 0);
		lminor = (minor_t)val;
		if (sdev_zvol_name2minor(dsname, &ominor) < 0 ||
		    ominor != lminor) {
			kmem_free(dsname, strlen(dsname) + 1);
			return (SDEV_VTOR_STALE);
		}
	}
	kmem_free(dsname, strlen(dsname) + 1);
	return (SDEV_VTOR_VALID);
}
