struct cifsFileInfo *find_readable_file(struct cifsInodeInfo *cifs_inode,
					bool fsuid_only)
{
	struct cifsFileInfo *open_file = NULL;
	struct cifs_sb_info *cifs_sb = CIFS_SB(cifs_inode->vfs_inode.i_sb);

	/* only filter by fsuid on multiuser mounts */
	if (!(cifs_sb->mnt_cifs_flags & CIFS_MOUNT_MULTIUSER))
		fsuid_only = false;

	spin_lock(&cifs_file_list_lock);
	/* we could simply get the first_list_entry since write-only entries
	   are always at the end of the list but since the first entry might
	   have a close pending, we go through the whole list */
	list_for_each_entry(open_file, &cifs_inode->openFileList, flist) {
		if (fsuid_only && !uid_eq(open_file->uid, current_fsuid()))
			continue;
		if (OPEN_FMODE(open_file->f_flags) & FMODE_READ) {
			if (!open_file->invalidHandle) {
				/* found a good file */
				/* lock it so it will not be closed on us */
				cifsFileInfo_get_locked(open_file);
				spin_unlock(&cifs_file_list_lock);
				return open_file;
			} /* else might as well continue, and look for
			     another, or simply have the caller reopen it
			     again rather than trying to fix this handle */
		} else /* write only file */
			break; /* write only files are last so must be done */
	}
	spin_unlock(&cifs_file_list_lock);
	return NULL;
}
