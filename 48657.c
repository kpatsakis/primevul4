devzvol_close_zfs()
{
	szcm = NULL;
	szn2m = NULL;
	(void) ldi_close(devzvol_lh, FREAD|FWRITE, kcred);
	ldi_ident_release(devzvol_li);
	if (zfs_mod != NULL) {
		(void) ddi_modclose(zfs_mod);
		zfs_mod = NULL;
	}
}
