static void __exit fini(void)
{
	reclaim_dma_bufs();

	unregister_virtio_driver(&virtio_console);
	unregister_virtio_driver(&virtio_rproc_serial);

	class_destroy(pdrvdata.class);
	debugfs_remove_recursive(pdrvdata.debugfs_dir);
}
