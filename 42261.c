static int __cpuinit msr_device_create(int cpu)
{
	struct device *dev;

	dev = device_create(msr_class, NULL, MKDEV(MSR_MAJOR, cpu), NULL,
			    "msr%d", cpu);
	return IS_ERR(dev) ? PTR_ERR(dev) : 0;
}
