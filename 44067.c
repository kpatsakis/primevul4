static int unlock_futex_pi(u32 __user *uaddr, u32 uval)
{
	u32 uninitialized_var(oldval);

	/*
	 * There is no waiter, so we unlock the futex. The owner died
	 * bit has not to be preserved here. We are the owner:
	 */
	if (cmpxchg_futex_value_locked(&oldval, uaddr, uval, 0))
		return -EFAULT;
	if (oldval != uval)
		return -EAGAIN;

	return 0;
}
