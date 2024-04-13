SYSCALL_DEFINE1(oldumount, char __user *, name)
{
	return sys_umount(name, 0);
}
