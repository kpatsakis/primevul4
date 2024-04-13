void set_mm_exe_file(struct mm_struct *mm, struct file *new_exe_file)
{
	struct file *old_exe_file;

	/*
	 * It is safe to dereference the exe_file without RCU as
	 * this function is only called if nobody else can access
	 * this mm -- see comment above for justification.
	 */
	old_exe_file = rcu_dereference_raw(mm->exe_file);

	if (new_exe_file)
		get_file(new_exe_file);
	rcu_assign_pointer(mm->exe_file, new_exe_file);
	if (old_exe_file)
		fput(old_exe_file);
}
