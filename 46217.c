static void free_sa_defrag_extent(struct new_sa_defrag_extent *new)
{
	struct old_sa_defrag_extent *old, *tmp;

	if (!new)
		return;

	list_for_each_entry_safe(old, tmp, &new->head, list) {
		list_del(&old->list);
		kfree(old);
	}
	kfree(new);
}
