void __init_ldsem(struct ld_semaphore *sem, const char *name,
		  struct lock_class_key *key)
{
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	/*
	 * Make sure we are not reinitializing a held semaphore:
	 */
	debug_check_no_locks_freed((void *)sem, sizeof(*sem));
	lockdep_init_map(&sem->dep_map, name, key, 0);
#endif
	sem->count = LDSEM_UNLOCKED;
	sem->wait_readers = 0;
	raw_spin_lock_init(&sem->wait_lock);
	INIT_LIST_HEAD(&sem->read_wait);
	INIT_LIST_HEAD(&sem->write_wait);
}
