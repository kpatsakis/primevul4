static long snd_seq_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct snd_seq_client *client = file->private_data;

	if (snd_BUG_ON(!client))
		return -ENXIO;
		
	return snd_seq_do_ioctl(client, cmd, (void __user *) arg);
}
