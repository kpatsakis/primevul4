static unsigned mounts_poll(struct file *file, poll_table *wait)
{
	struct proc_mounts *p = file->private_data;
	struct mnt_namespace *ns = p->ns;
	unsigned res = POLLIN | POLLRDNORM;

	poll_wait(file, &ns->poll, wait);

	spin_lock(&vfsmount_lock);
	if (p->event != ns->event) {
		p->event = ns->event;
		res |= POLLERR | POLLPRI;
	}
	spin_unlock(&vfsmount_lock);

	return res;
}
