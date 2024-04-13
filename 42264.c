static int __init msr_init(void)
{
	int i, err = 0;
	i = 0;

	if (__register_chrdev(MSR_MAJOR, 0, NR_CPUS, "cpu/msr", &msr_fops)) {
		printk(KERN_ERR "msr: unable to get major %d for msr\n",
		       MSR_MAJOR);
		err = -EBUSY;
		goto out;
	}
	msr_class = class_create(THIS_MODULE, "msr");
	if (IS_ERR(msr_class)) {
		err = PTR_ERR(msr_class);
		goto out_chrdev;
	}
	msr_class->devnode = msr_devnode;
	get_online_cpus();
	for_each_online_cpu(i) {
		err = msr_device_create(i);
		if (err != 0)
			goto out_class;
	}
	register_hotcpu_notifier(&msr_class_cpu_notifier);
	put_online_cpus();

	err = 0;
	goto out;

out_class:
	i = 0;
	for_each_online_cpu(i)
		msr_device_destroy(i);
	put_online_cpus();
	class_destroy(msr_class);
out_chrdev:
	__unregister_chrdev(MSR_MAJOR, 0, NR_CPUS, "cpu/msr");
out:
	return err;
}
