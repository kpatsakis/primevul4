cifs_has_mand_locks(struct cifsInodeInfo *cinode)
{
	struct cifs_fid_locks *cur;
	bool has_locks = false;

	down_read(&cinode->lock_sem);
	list_for_each_entry(cur, &cinode->llist, llist) {
		if (!list_empty(&cur->locks)) {
			has_locks = true;
			break;
		}
	}
	up_read(&cinode->lock_sem);
	return has_locks;
}
