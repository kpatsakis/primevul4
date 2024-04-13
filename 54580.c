static int dup_mmap(struct mm_struct *mm, struct mm_struct *oldmm)
{
	down_write(&oldmm->mmap_sem);
	RCU_INIT_POINTER(mm->exe_file, get_mm_exe_file(oldmm));
	up_write(&oldmm->mmap_sem);
	return 0;
}
