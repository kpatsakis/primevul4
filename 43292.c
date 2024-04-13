follow_link(struct path *link, struct nameidata *nd, void **p)
{
	struct dentry *dentry = link->dentry;
	int error;
	char *s;

	BUG_ON(nd->flags & LOOKUP_RCU);

	if (link->mnt == nd->path.mnt)
		mntget(link->mnt);

	error = -ELOOP;
	if (unlikely(current->total_link_count >= 40))
		goto out_put_nd_path;

	cond_resched();
	current->total_link_count++;

	touch_atime(link);
	nd_set_link(nd, NULL);

	error = security_inode_follow_link(link->dentry, nd);
	if (error)
		goto out_put_nd_path;

	nd->last_type = LAST_BIND;
	*p = dentry->d_inode->i_op->follow_link(dentry, nd);
	error = PTR_ERR(*p);
	if (IS_ERR(*p))
		goto out_put_nd_path;

	error = 0;
	s = nd_get_link(nd);
	if (s) {
		if (unlikely(IS_ERR(s))) {
			path_put(&nd->path);
			put_link(nd, link, *p);
			return PTR_ERR(s);
		}
		if (*s == '/') {
			set_root(nd);
			path_put(&nd->path);
			nd->path = nd->root;
			path_get(&nd->root);
			nd->flags |= LOOKUP_JUMPED;
		}
		nd->inode = nd->path.dentry->d_inode;
		error = link_path_walk(s, nd);
		if (unlikely(error))
			put_link(nd, link, *p);
	}

	return error;

out_put_nd_path:
	*p = NULL;
	path_put(&nd->path);
	path_put(link);
	return error;
}
