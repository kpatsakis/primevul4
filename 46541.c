static int follow_dotdot_rcu(struct nameidata *nd)
{
	struct inode *inode = nd->inode;
	if (!nd->root.mnt)
		set_root_rcu(nd);

	while (1) {
		if (nd->path.dentry == nd->root.dentry &&
		    nd->path.mnt == nd->root.mnt) {
			break;
		}
		if (nd->path.dentry != nd->path.mnt->mnt_root) {
			struct dentry *old = nd->path.dentry;
			struct dentry *parent = old->d_parent;
			unsigned seq;

			inode = parent->d_inode;
			seq = read_seqcount_begin(&parent->d_seq);
			if (read_seqcount_retry(&old->d_seq, nd->seq))
				goto failed;
			nd->path.dentry = parent;
			nd->seq = seq;
			break;
		}
		if (!follow_up_rcu(&nd->path))
			break;
		inode = nd->path.dentry->d_inode;
		nd->seq = read_seqcount_begin(&nd->path.dentry->d_seq);
	}
	while (d_mountpoint(nd->path.dentry)) {
		struct mount *mounted;
		mounted = __lookup_mnt(nd->path.mnt, nd->path.dentry);
		if (!mounted)
			break;
		nd->path.mnt = &mounted->mnt;
		nd->path.dentry = mounted->mnt.mnt_root;
		inode = nd->path.dentry->d_inode;
		nd->seq = read_seqcount_begin(&nd->path.dentry->d_seq);
		if (read_seqretry(&mount_lock, nd->m_seq))
			goto failed;
	}
	nd->inode = inode;
	return 0;

failed:
	nd->flags &= ~LOOKUP_RCU;
	if (!(nd->flags & LOOKUP_ROOT))
		nd->root.mnt = NULL;
	rcu_read_unlock();
	return -ECHILD;
}
