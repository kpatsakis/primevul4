int restore_altstack(const stack_t __user *uss)
{
	int err = do_sigaltstack(uss, NULL, current_user_stack_pointer());
	/* squash all but EFAULT for now */
	return err == -EFAULT ? err : 0;
}
