int generic_ptrace_pokedata(struct task_struct *tsk, unsigned long addr,
			    unsigned long data)
{
	int copied;

	copied = access_process_vm(tsk, addr, &data, sizeof(data), 1);
	return (copied == sizeof(data)) ? 0 : -EIO;
}
