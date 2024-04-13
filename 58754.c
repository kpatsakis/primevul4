static __init int fsnotify_init(void)
{
	int ret;

	BUG_ON(hweight32(ALL_FSNOTIFY_EVENTS) != 23);

	ret = init_srcu_struct(&fsnotify_mark_srcu);
	if (ret)
		panic("initializing fsnotify_mark_srcu");

	fsnotify_mark_connector_cachep = KMEM_CACHE(fsnotify_mark_connector,
						    SLAB_PANIC);

	return 0;
}
