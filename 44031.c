static int cmpxchg_futex_value_locked(u32 *curval, u32 __user *uaddr,
				      u32 uval, u32 newval)
{
	int ret;

	pagefault_disable();
	ret = futex_atomic_cmpxchg_inatomic(curval, uaddr, uval, newval);
	pagefault_enable();

	return ret;
}
