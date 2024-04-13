static int __init yam_init_driver(void)
{
	struct net_device *dev;
	int i, err;
	char name[IFNAMSIZ];

	printk(yam_drvinfo);

	for (i = 0; i < NR_PORTS; i++) {
		sprintf(name, "yam%d", i);
		
		dev = alloc_netdev(sizeof(struct yam_port), name,
				   yam_setup);
		if (!dev) {
			pr_err("yam: cannot allocate net device\n");
			err = -ENOMEM;
			goto error;
		}
		
		err = register_netdev(dev);
		if (err) {
			printk(KERN_WARNING "yam: cannot register net device %s\n", dev->name);
			goto error;
		}
		yam_devs[i] = dev;

	}

	yam_timer.function = yam_dotimer;
	yam_timer.expires = jiffies + HZ / 100;
	add_timer(&yam_timer);

	proc_create("yam", S_IRUGO, init_net.proc_net, &yam_info_fops);
	return 0;
 error:
	while (--i >= 0) {
		unregister_netdev(yam_devs[i]);
		free_netdev(yam_devs[i]);
	}
	return err;
}
