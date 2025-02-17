SYSCALL_DEFINE3(execve,
		const char __user *, filename,
		const char __user *const __user *, argv,
		const char __user *const __user *, envp)
{
	struct filename *path = getname(filename);
	int error = PTR_ERR(path);
	if (!IS_ERR(path)) {
		error = do_execve(path->name, argv, envp);
		putname(path);
	}
	return error;
}
