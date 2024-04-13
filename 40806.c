int compat_restore_altstack(const compat_stack_t __user *uss)
{
	int err = compat_sys_sigaltstack(uss, NULL);
	/* squash all but -EFAULT for now */
	return err == -EFAULT ? err : 0;
}
