cifs_umount(struct super_block *sb, struct cifs_sb_info *cifs_sb)
{
	int rc = 0;
	char *tmp;

	if (cifs_sb->tcon)
		cifs_put_tcon(cifs_sb->tcon);

	cifs_sb->tcon = NULL;
	tmp = cifs_sb->prepath;
	cifs_sb->prepathlen = 0;
	cifs_sb->prepath = NULL;
	kfree(tmp);

	return rc;
}
