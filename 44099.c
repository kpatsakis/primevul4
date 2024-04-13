static void __munlock_isolation_failed(struct page *page)
{
	if (PageUnevictable(page))
		__count_vm_event(UNEVICTABLE_PGSTRANDED);
	else
		__count_vm_event(UNEVICTABLE_PGMUNLOCKED);
}
