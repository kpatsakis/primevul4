devzvol_open_zfs()
{
	int rc;
	dev_t dv;

	devzvol_li = ldi_ident_from_anon();
	if (ldi_open_by_name("/dev/zfs", FREAD | FWRITE, kcred,
	    &devzvol_lh, devzvol_li))
		return (-1);
	if (zfs_mod == NULL && ((zfs_mod = ddi_modopen("fs/zfs",
	    KRTLD_MODE_FIRST, &rc)) == NULL)) {
		return (rc);
	}
	ASSERT(szcm == NULL && szn2m == NULL);
	if ((szcm = (int (*)(char *))
	    ddi_modsym(zfs_mod, "zvol_create_minor", &rc)) == NULL) {
		cmn_err(CE_WARN, "couldn't resolve zvol_create_minor");
		return (rc);
	}
	if ((szn2m = (int(*)(char *, minor_t *))
	    ddi_modsym(zfs_mod, "zvol_name2minor", &rc)) == NULL) {
		cmn_err(CE_WARN, "couldn't resolve zvol_name2minor");
		return (rc);
	}
	if (ldi_get_dev(devzvol_lh, &dv))
		return (-1);
	devzvol_major = getmajor(dv);
	return (0);
}
