void removed_exe_file_vma(struct mm_struct *mm)
{
	mm->num_exe_file_vmas--;
	if ((mm->num_exe_file_vmas == 0) && mm->exe_file){
		fput(mm->exe_file);
		mm->exe_file = NULL;
	}

}
