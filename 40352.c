static void *fb_seq_start(struct seq_file *m, loff_t *pos)
{
	mutex_lock(&registration_lock);
	return (*pos < FB_MAX) ? pos : NULL;
}
