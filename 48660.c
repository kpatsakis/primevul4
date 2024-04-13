devzvol_create_link(struct sdev_node *ddv, char *nm,
    void **arg, cred_t *cred, void *whatever, char *whichever)
{
	minor_t minor;
	char *pathname = (char *)*arg;
	int rc;
	char *dsname;
	char *x;
	char str[MAXNAMELEN];
	sdcmn_err13(("create_link (%s) (%s) '%s'", ddv->sdev_name,
	    ddv->sdev_path, nm));
	dsname = devzvol_make_dsname(ddv->sdev_path, nm);
	rc = sdev_zvol_create_minor(dsname);
	if ((rc != 0 && rc != EEXIST && rc != EBUSY) ||
	    sdev_zvol_name2minor(dsname, &minor)) {
		sdcmn_err13(("devzvol_create_link %d", rc));
		kmem_free(dsname, strlen(dsname) + 1);
		return (-1);
	}
	kmem_free(dsname, strlen(dsname) + 1);

	/*
	 * This is a valid zvol; create a symlink that points to the
	 * minor which was created under /devices/pseudo/zfs@0
	 */
	*pathname = '\0';
	for (x = ddv->sdev_path; x = strchr(x, '/'); x++)
		(void) strcat(pathname, "../");
	(void) snprintf(str, sizeof (str), ZVOL_PSEUDO_DEV "%u", minor);
	(void) strncat(pathname, str, MAXPATHLEN);
	if (strncmp(ddv->sdev_path, ZVOL_FULL_RDEV_DIR,
	    strlen(ZVOL_FULL_RDEV_DIR)) == 0)
		(void) strcat(pathname, ",raw");
	return (0);
}
