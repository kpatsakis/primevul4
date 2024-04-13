struct vfsmount *collect_mounts(struct path *path)
{
	struct mount *tree;
	down_write(&namespace_sem);
	tree = copy_tree(real_mount(path->mnt), path->dentry,
			 CL_COPY_ALL | CL_PRIVATE);
	up_write(&namespace_sem);
	if (IS_ERR(tree))
		return NULL;
	return &tree->mnt;
}
