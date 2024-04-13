static void path_cleanup(struct nameidata *nd)
{
	if (nd->root.mnt && !(nd->flags & LOOKUP_ROOT)) {
		path_put(&nd->root);
		nd->root.mnt = NULL;
	}
	if (unlikely(nd->base))
		fput(nd->base);
}
