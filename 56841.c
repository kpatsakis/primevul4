SYSCALL_DEFINE1(mq_unlink, const char __user *, u_name)
{
	int err;
	struct filename *name;
	struct dentry *dentry;
	struct inode *inode = NULL;
	struct ipc_namespace *ipc_ns = current->nsproxy->ipc_ns;
	struct vfsmount *mnt = ipc_ns->mq_mnt;

	name = getname(u_name);
	if (IS_ERR(name))
		return PTR_ERR(name);

	audit_inode_parent_hidden(name, mnt->mnt_root);
	err = mnt_want_write(mnt);
	if (err)
		goto out_name;
	inode_lock_nested(d_inode(mnt->mnt_root), I_MUTEX_PARENT);
	dentry = lookup_one_len(name->name, mnt->mnt_root,
				strlen(name->name));
	if (IS_ERR(dentry)) {
		err = PTR_ERR(dentry);
		goto out_unlock;
	}

	inode = d_inode(dentry);
	if (!inode) {
		err = -ENOENT;
	} else {
		ihold(inode);
		err = vfs_unlink(d_inode(dentry->d_parent), dentry, NULL);
	}
	dput(dentry);

out_unlock:
	inode_unlock(d_inode(mnt->mnt_root));
	if (inode)
		iput(inode);
	mnt_drop_write(mnt);
out_name:
	putname(name);

	return err;
}
