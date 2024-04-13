static int key_wait_bit(void *flags)
{
	schedule();
	return 0;
}
