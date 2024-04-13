struct vfsmount *lookup_mnt(struct path *path)
{
	struct mount *child_mnt;

	br_read_lock(&vfsmount_lock);
	child_mnt = __lookup_mnt(path->mnt, path->dentry, 1);
	if (child_mnt) {
		mnt_add_count(child_mnt, 1);
		br_read_unlock(&vfsmount_lock);
		return &child_mnt->mnt;
	} else {
		br_read_unlock(&vfsmount_lock);
		return NULL;
	}
}
