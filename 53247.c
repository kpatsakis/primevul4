static inline void clear_soft_dirty_pmd(struct vm_area_struct *vma,
		unsigned long addr, pmd_t *pmdp)
{
	pmd_t pmd = *pmdp;

	pmd = pmd_wrprotect(pmd);
	pmd = pmd_clear_flags(pmd, _PAGE_SOFT_DIRTY);

	if (vma->vm_flags & VM_SOFTDIRTY)
		vma->vm_flags &= ~VM_SOFTDIRTY;

	set_pmd_at(vma->vm_mm, addr, pmdp, pmd);
}
