cifs_find_fid_lock_conflict(struct cifs_fid_locks *fdlocks, __u64 offset,
			    __u64 length, __u8 type, struct cifsFileInfo *cfile,
			    struct cifsLockInfo **conf_lock, int rw_check)
{
	struct cifsLockInfo *li;
	struct cifsFileInfo *cur_cfile = fdlocks->cfile;
	struct TCP_Server_Info *server = tlink_tcon(cfile->tlink)->ses->server;

	list_for_each_entry(li, &fdlocks->locks, llist) {
		if (offset + length <= li->offset ||
		    offset >= li->offset + li->length)
			continue;
		if (rw_check != CIFS_LOCK_OP && current->tgid == li->pid &&
		    server->ops->compare_fids(cfile, cur_cfile)) {
			/* shared lock prevents write op through the same fid */
			if (!(li->type & server->vals->shared_lock_type) ||
			    rw_check != CIFS_WRITE_OP)
				continue;
		}
		if ((type & server->vals->shared_lock_type) &&
		    ((server->ops->compare_fids(cfile, cur_cfile) &&
		     current->tgid == li->pid) || type == li->type))
			continue;
		if (conf_lock)
			*conf_lock = li;
		return true;
	}
	return false;
}
