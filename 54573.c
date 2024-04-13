static void check_mm(struct mm_struct *mm)
{
	int i;

	for (i = 0; i < NR_MM_COUNTERS; i++) {
		long x = atomic_long_read(&mm->rss_stat.count[i]);

		if (unlikely(x))
			printk(KERN_ALERT "BUG: Bad rss-counter state "
					  "mm:%p idx:%d val:%ld\n", mm, i, x);
	}

	if (atomic_long_read(&mm->nr_ptes))
		pr_alert("BUG: non-zero nr_ptes on freeing mm: %ld\n",
				atomic_long_read(&mm->nr_ptes));
	if (mm_nr_pmds(mm))
		pr_alert("BUG: non-zero nr_pmds on freeing mm: %ld\n",
				mm_nr_pmds(mm));

#if defined(CONFIG_TRANSPARENT_HUGEPAGE) && !USE_SPLIT_PMD_PTLOCKS
	VM_BUG_ON_MM(mm->pmd_huge_pte, mm);
#endif
}
