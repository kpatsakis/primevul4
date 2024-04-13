static void __exit cuse_exit(void)
{
	misc_deregister(&cuse_miscdev);
	class_destroy(cuse_class);
}
