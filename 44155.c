void unlink_anon_vmas(struct vm_area_struct *vma)
{
	struct anon_vma_chain *avc, *next;
	struct anon_vma *root = NULL;

	/*
	 * Unlink each anon_vma chained to the VMA.  This list is ordered
	 * from newest to oldest, ensuring the root anon_vma gets freed last.
	 */
	list_for_each_entry_safe(avc, next, &vma->anon_vma_chain, same_vma) {
		struct anon_vma *anon_vma = avc->anon_vma;

		root = lock_anon_vma_root(root, anon_vma);
		anon_vma_interval_tree_remove(avc, &anon_vma->rb_root);

		/*
		 * Leave empty anon_vmas on the list - we'll need
		 * to free them outside the lock.
		 */
		if (RB_EMPTY_ROOT(&anon_vma->rb_root))
			continue;

		list_del(&avc->same_vma);
		anon_vma_chain_free(avc);
	}
	unlock_anon_vma_root(root);

	/*
	 * Iterate the list once more, it now only contains empty and unlinked
	 * anon_vmas, destroy them. Could not do before due to __put_anon_vma()
	 * needing to write-acquire the anon_vma->root->rwsem.
	 */
	list_for_each_entry_safe(avc, next, &vma->anon_vma_chain, same_vma) {
		struct anon_vma *anon_vma = avc->anon_vma;

		put_anon_vma(anon_vma);

		list_del(&avc->same_vma);
		anon_vma_chain_free(avc);
	}
}
