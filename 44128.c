static bool invalid_migration_vma(struct vm_area_struct *vma, void *arg)
{
	return is_vma_temporary_stack(vma);
}
