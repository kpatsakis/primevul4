SYSCALL_DEFINE0(restart_syscall)
{
	struct restart_block *restart = &current_thread_info()->restart_block;
	return restart->fn(restart);
}
