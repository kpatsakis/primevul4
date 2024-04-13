static void msr_device_destroy(int cpu)
{
	device_destroy(msr_class, MKDEV(MSR_MAJOR, cpu));
}
