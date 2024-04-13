int cifs_fsync(struct file *file, loff_t start, loff_t end, int datasync)
{
	unsigned int xid;
	int rc = 0;
	struct cifs_tcon *tcon;
	struct TCP_Server_Info *server;
	struct cifsFileInfo *smbfile = file->private_data;
	struct cifs_sb_info *cifs_sb = CIFS_SB(file->f_path.dentry->d_sb);
	struct inode *inode = file->f_mapping->host;

	rc = filemap_write_and_wait_range(inode->i_mapping, start, end);
	if (rc)
		return rc;
	mutex_lock(&inode->i_mutex);

	xid = get_xid();

	cifs_dbg(FYI, "Sync file - name: %s datasync: 0x%x\n",
		 file->f_path.dentry->d_name.name, datasync);

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
