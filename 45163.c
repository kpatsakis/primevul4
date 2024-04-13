cifs_read(struct file *file, char *read_data, size_t read_size, loff_t *offset)
{
	int rc = -EACCES;
	unsigned int bytes_read = 0;
	unsigned int total_read;
	unsigned int current_read_size;
	unsigned int rsize;
	struct cifs_sb_info *cifs_sb;
	struct cifs_tcon *tcon;
	struct TCP_Server_Info *server;
	unsigned int xid;
	char *cur_offset;
	struct cifsFileInfo *open_file;
	struct cifs_io_parms io_parms;
	int buf_type = CIFS_NO_BUFFER;
	__u32 pid;

	xid = get_xid();
	cifs_sb = CIFS_SB(file->f_path.dentry->d_sb);

	/* FIXME: set up handlers for larger reads and/or convert to async */
	rsize = min_t(unsigned int, cifs_sb->rsize, CIFSMaxBufSize);

	if (file->private_data == NULL) {
		rc = -EBADF;
		free_xid(xid);
		return rc;
	}
	open_file = file->private_data;
	tcon = tlink_tcon(open_file->tlink);
	server = tcon->ses->server;

	if (!server->ops->sync_read) {
		free_xid(xid);
		return -ENOSYS;
	}

	if (cifs_sb->mnt_cifs_flags & CIFS_MOUNT_RWPIDFORWARD)
		pid = open_file->pid;
	else
		pid = current->tgid;

	if ((file->f_flags & O_ACCMODE) == O_WRONLY)
		cifs_dbg(FYI, "attempting read on write only file instance\n");

	for (total_read = 0, cur_offset = read_data; read_size > total_read;
	     total_read += bytes_read, cur_offset += bytes_read) {
		current_read_size = min_t(uint, read_size - total_read, rsize);
		/*
		 * For windows me and 9x we do not want to request more than it
		 * negotiated since it will refuse the read then.
		 */
		if ((tcon->ses) && !(tcon->ses->capabilities &
				tcon->ses->server->vals->cap_large_files)) {
			current_read_size = min_t(uint, current_read_size,
					CIFSMaxBufSize);
		}
		rc = -EAGAIN;
		while (rc == -EAGAIN) {
			if (open_file->invalidHandle) {
				rc = cifs_reopen_file(open_file, true);
				if (rc != 0)
					break;
			}
			io_parms.pid = pid;
			io_parms.tcon = tcon;
			io_parms.offset = *offset;
			io_parms.length = current_read_size;
			rc = server->ops->sync_read(xid, open_file, &io_parms,
						    &bytes_read, &cur_offset,
						    &buf_type);
		}
		if (rc || (bytes_read == 0)) {
			if (total_read) {
				break;
			} else {
				free_xid(xid);
				return rc;
			}
		} else {
			cifs_stats_bytes_read(tcon, total_read);
			*offset += bytes_read;
		}
	}
	free_xid(xid);
	return total_read;
}
