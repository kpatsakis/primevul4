void __init signals_init(void)
{
	sigqueue_cachep = KMEM_CACHE(sigqueue, SLAB_PANIC);
}
