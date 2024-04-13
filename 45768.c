static int __vsock_core_init(void)
{
	int err;

	vsock_init_tables();

	vsock_device.minor = MISC_DYNAMIC_MINOR;
	err = misc_register(&vsock_device);
	if (err) {
		pr_err("Failed to register misc device\n");
		return -ENOENT;
	}

	err = proto_register(&vsock_proto, 1);	/* we want our slab */
	if (err) {
		pr_err("Cannot register vsock protocol\n");
		goto err_misc_deregister;
	}

	err = sock_register(&vsock_family_ops);
	if (err) {
		pr_err("could not register af_vsock (%d) address family: %d\n",
		       AF_VSOCK, err);
		goto err_unregister_proto;
	}

	return 0;

err_unregister_proto:
	proto_unregister(&vsock_proto);
err_misc_deregister:
	misc_deregister(&vsock_device);
	return err;
}
