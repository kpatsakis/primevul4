static unsigned int snd_timer_user_poll(struct file *file, poll_table * wait)
{
        unsigned int mask;
        struct snd_timer_user *tu;

        tu = file->private_data;

        poll_wait(file, &tu->qchange_sleep, wait);

	mask = 0;
	if (tu->qused)
		mask |= POLLIN | POLLRDNORM;

	return mask;
}
