static void update_dev_time(const char *path_name)
{
	struct file *filp;

	filp = filp_open(path_name, O_RDWR, 0);
	if (IS_ERR(filp))
		return;
	file_update_time(filp);
	filp_close(filp, NULL);
}