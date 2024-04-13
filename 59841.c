static int key_wait_bit_intr(void *flags)
{
	schedule();
	return signal_pending(current) ? -ERESTARTSYS : 0;
}
