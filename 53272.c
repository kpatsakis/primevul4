static int show_numa_map(struct seq_file *m, void *v, int is_pid)
{
	struct numa_maps_private *numa_priv = m->private;
	struct proc_maps_private *proc_priv = &numa_priv->proc_maps;
	struct vm_area_struct *vma = v;
	struct numa_maps *md = &numa_priv->md;
	struct file *file = vma->vm_file;
	struct mm_struct *mm = vma->vm_mm;
	struct mm_walk walk = {
		.hugetlb_entry = gather_hugetlb_stats,
		.pmd_entry = gather_pte_stats,
		.private = md,
		.mm = mm,
	};
	struct mempolicy *pol;
	char buffer[64];
	int nid;

	if (!mm)
		return 0;

	/* Ensure we start with an empty set of numa_maps statistics. */
	memset(md, 0, sizeof(*md));

	pol = __get_vma_policy(vma, vma->vm_start);
	if (pol) {
		mpol_to_str(buffer, sizeof(buffer), pol);
		mpol_cond_put(pol);
	} else {
		mpol_to_str(buffer, sizeof(buffer), proc_priv->task_mempolicy);
	}

	seq_printf(m, "%08lx %s", vma->vm_start, buffer);

	if (file) {
		seq_puts(m, " file=");
		seq_path(m, &file->f_path, "\n\t= ");
	} else if (vma->vm_start <= mm->brk && vma->vm_end >= mm->start_brk) {
		seq_puts(m, " heap");
	} else {
		pid_t tid = pid_of_stack(proc_priv, vma, is_pid);
		if (tid != 0) {
			/*
			 * Thread stack in /proc/PID/task/TID/maps or
			 * the main process stack.
			 */
			if (!is_pid || (vma->vm_start <= mm->start_stack &&
			    vma->vm_end >= mm->start_stack))
				seq_puts(m, " stack");
			else
				seq_printf(m, " stack:%d", tid);
		}
	}

	if (is_vm_hugetlb_page(vma))
		seq_puts(m, " huge");

	/* mmap_sem is held by m_start */
	walk_page_vma(vma, &walk);

	if (!md->pages)
		goto out;

	if (md->anon)
		seq_printf(m, " anon=%lu", md->anon);

	if (md->dirty)
		seq_printf(m, " dirty=%lu", md->dirty);

	if (md->pages != md->anon && md->pages != md->dirty)
		seq_printf(m, " mapped=%lu", md->pages);

	if (md->mapcount_max > 1)
		seq_printf(m, " mapmax=%lu", md->mapcount_max);

	if (md->swapcache)
		seq_printf(m, " swapcache=%lu", md->swapcache);

	if (md->active < md->pages && !is_vm_hugetlb_page(vma))
		seq_printf(m, " active=%lu", md->active);

	if (md->writeback)
		seq_printf(m, " writeback=%lu", md->writeback);

	for_each_node_state(nid, N_MEMORY)
		if (md->node[nid])
			seq_printf(m, " N%d=%lu", nid, md->node[nid]);

	seq_printf(m, " kernelpagesize_kB=%lu", vma_kernel_pagesize(vma) >> 10);
out:
	seq_putc(m, '\n');
	m_cache_vma(m, vma);
	return 0;
}
