SYSCALL_DEFINE2(sigaltstack,const stack_t __user *,uss, stack_t __user *,uoss)
{
	return do_sigaltstack(uss, uoss, current_user_stack_pointer());
}
