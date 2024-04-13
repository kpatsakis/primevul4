static int path_init(int dfd, const char *name, unsigned int flags,
		     struct nameidata *nd, struct file **fp)
{
	int retval = 0;

	nd->last_type = LAST_ROOT; /* if there are only slashes... */
	nd->flags = flags | LOOKUP_JUMPED;
	nd->depth = 0;
	if (flags & LOOKUP_ROOT) {
		struct dentry *root = nd->root.dentry;
		struct inode *inode = root->d_inode;
		if (*name) {
			if (!d_can_lookup(root))
				return -ENOTDIR;
			retval = inode_permission(inode, MAY_EXEC);
			if (retval)
				return retval;
		}
		nd->path = nd->root;
		nd->inode = inode;
		if (flags & LOOKUP_RCU) {
			rcu_read_lock();
			nd->seq = __read_seqcount_begin(&nd->path.dentry->d_seq);
			nd->m_seq = read_seqbegin(&mount_lock);
		} else {
			path_get(&nd->path);
		}
		return 0;
	}

	nd->root.mnt = NULL;

	nd->m_seq = read_seqbegin(&mount_lock);
	if (*name=='/') {
		if (flags & LOOKUP_RCU) {
			rcu_read_lock();
			set_root_rcu(nd);
		} else {
			set_root(nd);
			path_get(&nd->root);
		}
		nd->path = nd->root;
	} else if (dfd == AT_FDCWD) {
		if (flags & LOOKUP_RCU) {
			struct fs_struct *fs = current->fs;
			unsigned seq;

			rcu_read_lock();

			do {
				seq = read_seqcount_begin(&fs->seq);
				nd->path = fs->pwd;
				nd->seq = __read_seqcount_begin(&nd->path.dentry->d_seq);
			} while (read_seqcount_retry(&fs->seq, seq));
		} else {
			get_fs_pwd(current->fs, &nd->path);
		}
	} else {
		/* Caller must check execute permissions on the starting path component */
		struct fd f = fdget_raw(dfd);
		struct dentry *dentry;

		if (!f.file)
			return -EBADF;

		dentry = f.file->f_path.dentry;

		if (*name) {
			if (!d_can_lookup(dentry)) {
				fdput(f);
				return -ENOTDIR;
			}
		}

		nd->path = f.file->f_path;
		if (flags & LOOKUP_RCU) {
			if (f.flags & FDPUT_FPUT)
				*fp = f.file;
			nd->seq = __read_seqcount_begin(&nd->path.dentry->d_seq);
			rcu_read_lock();
		} else {
			path_get(&nd->path);
			fdput(f);
		}
	}

	nd->inode = nd->path.dentry->d_inode;
	return 0;
}
