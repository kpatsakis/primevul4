static __always_inline void set_root(struct nameidata *nd)
{
	get_fs_root(current->fs, &nd->root);
}
