static int __init init(void)
{
	int err;

	pdrvdata.class = class_create(THIS_MODULE, "virtio-ports");
	if (IS_ERR(pdrvdata.class)) {
		err = PTR_ERR(pdrvdata.class);
		pr_err("Error %d creating virtio-ports class\n", err);
		return err;
	}

	pdrvdata.debugfs_dir = debugfs_create_dir("virtio-ports", NULL);
	if (!pdrvdata.debugfs_dir)
		pr_warning("Error creating debugfs dir for virtio-ports\n");
	INIT_LIST_HEAD(&pdrvdata.consoles);
	INIT_LIST_HEAD(&pdrvdata.portdevs);

	err = register_virtio_driver(&virtio_console);
	if (err < 0) {
		pr_err("Error %d registering virtio driver\n", err);
		goto free;
	}
	err = register_virtio_driver(&virtio_rproc_serial);
	if (err < 0) {
		pr_err("Error %d registering virtio rproc serial driver\n",
		       err);
		goto unregister;
	}
	return 0;
unregister:
	unregister_virtio_driver(&virtio_console);
free:
	debugfs_remove_recursive(pdrvdata.debugfs_dir);
	class_destroy(pdrvdata.class);
	return err;
}
