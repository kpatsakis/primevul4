static int shmem_free_swap(struct address_space *mapping,
			   pgoff_t index, void *radswap)
{
	int error;

	spin_lock_irq(&mapping->tree_lock);
	error = shmem_radix_tree_replace(mapping, index, radswap, NULL);
	spin_unlock_irq(&mapping->tree_lock);
	if (!error)
		free_swap_and_cache(radix_to_swp_entry(radswap));
	return error;
}
