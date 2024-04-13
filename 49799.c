static void adf_chr_drv_destroy(void)
{
	device_destroy(adt_ctl_drv.drv_class, MKDEV(adt_ctl_drv.major, 0));
	cdev_del(&adt_ctl_drv.drv_cdev);
	class_destroy(adt_ctl_drv.drv_class);
	unregister_chrdev_region(MKDEV(adt_ctl_drv.major, 0), 1);
}
