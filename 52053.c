static void usbfs_decrease_memory_usage(unsigned amount)
{
	atomic_sub(amount, &usbfs_memory_usage);
}
