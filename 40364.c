fbmem_exit(void)
{
	remove_proc_entry("fb", NULL);
	class_destroy(fb_class);
	unregister_chrdev(FB_MAJOR, "fb");
}
