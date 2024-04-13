cifs_setlk(struct file *file, struct file_lock *flock, __u32 type,
	   bool wait_flag, bool posix_lck, int lock, int unlock,
	   unsigned int xid)
{
	int rc = 0;
	__u64 length = 1 + flock->fl_end - flock->fl_start;
	struct cifsFileInfo *cfile = (struct cifsFileInfo *)file->private_data;
	struct cifs_tcon *tcon = tlink_tcon(cfile->tlink);
	struct TCP_Server_Info *server = tcon->ses->server;
	struct inode *inode = cfile->dentry->d_inode;

	if (posix_lck) {
		int posix_lock_type;

		rc = cifs_posix_lock_set(file, flock);
		if (!rc || rc < 0)
			return rc;

		if (type & server->vals->shared_lock_type)
			posix_lock_type = CIFS_RDLCK;
		else
			posix_lock_type = CIFS_WRLCK;

		if (unlock == 1)
			posix_lock_type = CIFS_UNLCK;

		rc = CIFSSMBPosixLock(xid, tcon, cfile->fid.netfid,
				      current->tgid, flock->fl_start, length,
				      NULL, posix_lock_type, wait_flag);
		goto out;
	}

	if (lock) {
		struct cifsLockInfo *lock;

		lock = cifs_lock_init(flock->fl_start, length, type);
		if (!lock)
			return -ENOMEM;

		rc = cifs_lock_add_if(cfile, lock, wait_flag);
		if (rc < 0) {
			kfree(lock);
			return rc;
		}
		if (!rc)
			goto out;

		/*
		 * Windows 7 server can delay breaking lease from read to None
		 * if we set a byte-range lock on a file - break it explicitly
		 * before sending the lock to the server to be sure the next
		 * read won't conflict with non-overlapted locks due to
		 * pagereading.
		 */
		if (!CIFS_CACHE_WRITE(CIFS_I(inode)) &&
					CIFS_CACHE_READ(CIFS_I(inode))) {
			cifs_invalidate_mapping(inode);
			cifs_dbg(FYI, "Set no oplock for inode=%p due to mand locks\n",
				 inode);
			CIFS_I(inode)->oplock = 0;
		}

		rc = server->ops->mand_lock(xid, cfile, flock->fl_start, length,
					    type, 1, 0, wait_flag);
		if (rc) {
			kfree(lock);
			return rc;
		}

		cifs_lock_add(cfile, lock);
	} else if (unlock)
		rc = server->ops->mand_unlock_range(cfile, flock, xid);

out:
	if (flock->fl_flags & FL_POSIX)
		posix_lock_file_wait(file, flock);
	return rc;
}
