bm_status_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
	char *s = enabled ? "enabled\n" : "disabled\n";

	return simple_read_from_buffer(buf, nbytes, ppos, s, strlen(s));
}
