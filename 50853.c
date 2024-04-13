static int proc_pid_auxv(struct seq_file *m, struct pid_namespace *ns,
			 struct pid *pid, struct task_struct *task)
{
	struct mm_struct *mm = mm_access(task, PTRACE_MODE_READ_FSCREDS);
	if (mm && !IS_ERR(mm)) {
		unsigned int nwords = 0;
		do {
			nwords += 2;
		} while (mm->saved_auxv[nwords - 2] != 0); /* AT_NULL */
		seq_write(m, mm->saved_auxv, nwords * sizeof(mm->saved_auxv[0]));
		mmput(mm);
		return 0;
	} else
		return PTR_ERR(mm);
}
