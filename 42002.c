static int shmem_radix_tree_replace(struct address_space *mapping,
			pgoff_t index, void *expected, void *replacement)
{
	void **pslot;
	void *item = NULL;

	VM_BUG_ON(!expected);
	pslot = radix_tree_lookup_slot(&mapping->page_tree, index);
	if (pslot)
		item = radix_tree_deref_slot_protected(pslot,
							&mapping->tree_lock);
	if (item != expected)
		return -ENOENT;
	if (replacement)
		radix_tree_replace_slot(pslot, replacement);
	else
		radix_tree_delete(&mapping->page_tree, index);
	return 0;
}
