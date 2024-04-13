cifs_find_lock_conflict(struct cifsFileInfo *cfile, __u64 offset, __u64 length,
			__u8 type, struct cifsLockInfo **conf_lock,
			int rw_check)
{
	bool rc = false;
	struct cifs_fid_locks *cur;
	struct cifsInodeInfo *cinode = CIFS_I(cfile->dentry->d_inode);

	list_for_each_entry(cur, &cinode->llist, llist) {
		rc = cifs_find_fid_lock_conflict(cur, offset, length, type,
						 cfile, conf_lock, rw_check);
		if (rc)
			break;
	}

	return rc;
}
