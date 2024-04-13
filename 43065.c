asmlinkage __visible void kvm_spurious_fault(void)
{
	/* Fault while not rebooting.  We want the trace. */
	BUG();
}
