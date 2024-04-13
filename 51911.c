static int __init ppp_init(void)
{
	int err;

	pr_info("PPP generic driver version " PPP_VERSION "\n");

	err = register_pernet_device(&ppp_net_ops);
	if (err) {
		pr_err("failed to register PPP pernet device (%d)\n", err);
		goto out;
	}

	err = register_chrdev(PPP_MAJOR, "ppp", &ppp_device_fops);
	if (err) {
		pr_err("failed to register PPP device (%d)\n", err);
		goto out_net;
	}

	ppp_class = class_create(THIS_MODULE, "ppp");
	if (IS_ERR(ppp_class)) {
		err = PTR_ERR(ppp_class);
		goto out_chrdev;
	}

	/* not a big deal if we fail here :-) */
	device_create(ppp_class, NULL, MKDEV(PPP_MAJOR, 0), NULL, "ppp");

	return 0;

out_chrdev:
	unregister_chrdev(PPP_MAJOR, "ppp");
out_net:
	unregister_pernet_device(&ppp_net_ops);
out:
	return err;
}
