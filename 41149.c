struct mount *copy_tree(struct mount *mnt, struct dentry *dentry,
					int flag)
{
	struct mount *res, *p, *q, *r;
	struct path path;

	if (!(flag & CL_COPY_ALL) && IS_MNT_UNBINDABLE(mnt))
		return ERR_PTR(-EINVAL);

	res = q = clone_mnt(mnt, dentry, flag);
	if (IS_ERR(q))
		return q;

	q->mnt_mountpoint = mnt->mnt_mountpoint;

	p = mnt;
	list_for_each_entry(r, &mnt->mnt_mounts, mnt_child) {
		struct mount *s;
		if (!is_subdir(r->mnt_mountpoint, dentry))
			continue;

		for (s = r; s; s = next_mnt(s, r)) {
			if (!(flag & CL_COPY_ALL) && IS_MNT_UNBINDABLE(s)) {
				s = skip_mnt_tree(s);
				continue;
			}
			while (p != s->mnt_parent) {
				p = p->mnt_parent;
				q = q->mnt_parent;
			}
			p = s;
			path.mnt = &q->mnt;
			path.dentry = p->mnt_mountpoint;
			q = clone_mnt(p, p->mnt.mnt_root, flag);
			if (IS_ERR(q))
				goto out;
			br_write_lock(&vfsmount_lock);
			list_add_tail(&q->mnt_list, &res->mnt_list);
			attach_mnt(q, &path);
			br_write_unlock(&vfsmount_lock);
		}
	}
	return res;
out:
	if (res) {
		LIST_HEAD(umount_list);
		br_write_lock(&vfsmount_lock);
		umount_tree(res, 0, &umount_list);
		br_write_unlock(&vfsmount_lock);
		release_mounts(&umount_list);
	}
	return q;
}
