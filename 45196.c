struct cifsFileInfo *find_writable_file(struct cifsInodeInfo *cifs_inode,
					bool fsuid_only)
{
	struct cifsFileInfo *open_file, *inv_file = NULL;
	struct cifs_sb_info *cifs_sb;
	bool any_available = false;
	int rc;
	unsigned int refind = 0;

	/* Having a null inode here (because mapping->host was set to zero by
	the VFS or MM) should not happen but we had reports of on oops (due to
	it being zero) during stress testcases so we need to check for it */

	if (cifs_inode == NULL) {
		cifs_dbg(VFS, "Null inode passed to cifs_writeable_file\n");
		dump_stack();
		return NULL;
	}

	cifs_sb = CIFS_SB(cifs_inode->vfs_inode.i_sb);

	/* only filter by fsuid on multiuser mounts */
	if (!(cifs_sb->mnt_cifs_flags & CIFS_MOUNT_MULTIUSER))
		fsuid_only = false;

	spin_lock(&cifs_file_list_lock);
refind_writable:
	if (refind > MAX_REOPEN_ATT) {
		spin_unlock(&cifs_file_list_lock);
		return NULL;
	}
	list_for_each_entry(open_file, &cifs_inode->openFileList, flist) {
		if (!any_available && open_file->pid != current->tgid)
			continue;
		if (fsuid_only && !uid_eq(open_file->uid, current_fsuid()))
			continue;
		if (OPEN_FMODE(open_file->f_flags) & FMODE_WRITE) {
			if (!open_file->invalidHandle) {
				/* found a good writable file */
				cifsFileInfo_get_locked(open_file);
				spin_unlock(&cifs_file_list_lock);
				return open_file;
			} else {
				if (!inv_file)
					inv_file = open_file;
			}
		}
	}
	/* couldn't find useable FH with same pid, try any available */
	if (!any_available) {
		any_available = true;
		goto refind_writable;
	}

	if (inv_file) {
		any_available = false;
		cifsFileInfo_get_locked(inv_file);
	}

	spin_unlock(&cifs_file_list_lock);

	if (inv_file) {
		rc = cifs_reopen_file(inv_file, false);
		if (!rc)
			return inv_file;
		else {
			spin_lock(&cifs_file_list_lock);
			list_move_tail(&inv_file->flist,
					&cifs_inode->openFileList);
			spin_unlock(&cifs_file_list_lock);
			cifsFileInfo_put(inv_file);
			spin_lock(&cifs_file_list_lock);
			++refind;
			goto refind_writable;
		}
	}

	return NULL;
}
