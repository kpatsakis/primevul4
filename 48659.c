devzvol_create_dir(struct sdev_node *ddv, char *nm, void **arg,
    cred_t *cred, void *whatever, char *whichever)
{
	timestruc_t now;
	struct vattr *vap = (struct vattr *)arg;

	sdcmn_err13(("create_dir (%s) (%s) '%s'", ddv->sdev_name,
	    ddv->sdev_path, nm));
	ASSERT(strncmp(ddv->sdev_path, ZVOL_DIR,
	    strlen(ZVOL_DIR)) == 0);
	*vap = *sdev_getdefault_attr(VDIR);
	gethrestime(&now);
	vap->va_atime = now;
	vap->va_mtime = now;
	vap->va_ctime = now;
	return (0);
}
