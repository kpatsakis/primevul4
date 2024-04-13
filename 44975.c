static __always_inline void set_root(struct nameidata *nd)
{
	if (!nd->root.mnt) {
		struct fs_struct *fs = current->fs;
		read_lock(&fs->lock);
		nd->root = fs->root;
		path_get(&nd->root);
		read_unlock(&fs->lock);
	}
}
