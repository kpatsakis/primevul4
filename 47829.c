static bool always_dump_vma(struct vm_area_struct *vma)
{
	/* Any vsyscall mappings? */
	if (vma == get_gate_vma(vma->vm_mm))
		return true;

	/*
	 * Assume that all vmas with a .name op should always be dumped.
	 * If this changes, a new vm_ops field can easily be added.
	 */
	if (vma->vm_ops && vma->vm_ops->name && vma->vm_ops->name(vma))
		return true;

	/*
	 * arch_vma_name() returns non-NULL for special architecture mappings,
	 * such as vDSO sections.
	 */
	if (arch_vma_name(vma))
		return true;

	return false;
}
