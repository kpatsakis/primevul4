static u32 cmpxchg_futex_value_locked(u32 __user *uaddr, u32 uval, u32 newval)
{
	u32 curval;

	pagefault_disable();
	curval = futex_atomic_cmpxchg_inatomic(uaddr, uval, newval);
	pagefault_enable();

	return curval;
}
