int cifs_strict_fsync(struct file *file, loff_t start, loff_t end,
		      int datasync)
{
	unsigned int xid;
	int rc = 0;
	struct cifs_tcon *tcon;
	struct TCP_Server_Info *server;
	struct cifsFileInfo *smbfile = file->private_data;
	struct inode *inode = file_inode(file);
	struct cifs_sb_info *cifs_sb = CIFS_SB(inode->i_sb);

	rc = filemap_write_and_wait_range(inode->i_mapping, start, end);
	if (rc)
		return rc;
	mutex_lock(&inode->i_mutex);

	xid = get_xid();

	cifs_dbg(FYI, "Sync file - name: %s datasync: 0x%x\n",
		 file->f_path.dentry->d_name.name, datasync);

	if (!CIFS_CACHE_READ(CIFS_I(inode))) {
		rc = cifs_invalidate_mapping(inode);
		if (rc) {
			cifs_dbg(FYI, "rc: %d during invalidate phase\n", rc);
			rc = 0; /* don't care about it in fsync */
		}
	}

	tcon = tlink_tcon(smbfile->tlink);
	if (!(cifs_sb->mnt_cifs_flags & CIFS_MOUNT_NOSSYNC)) {
		server = tcon->ses->server;
		if (server->ops->flush)
			rc = server->ops->flush(xid, tcon, &smbfile->fid);
		else
			rc = -ENOSYS;
	}

	free_xid(xid);
	mutex_unlock(&inode->i_mutex);
	return rc;
}
