struct tty_struct *alloc_tty_struct(struct tty_driver *driver, int idx)
{
	struct tty_struct *tty;

	tty = kzalloc(sizeof(*tty), GFP_KERNEL);
	if (!tty)
		return NULL;

	kref_init(&tty->kref);
	tty->magic = TTY_MAGIC;
	tty_ldisc_init(tty);
	tty->session = NULL;
	tty->pgrp = NULL;
	mutex_init(&tty->legacy_mutex);
	mutex_init(&tty->throttle_mutex);
	init_rwsem(&tty->termios_rwsem);
	mutex_init(&tty->winsize_mutex);
	init_ldsem(&tty->ldisc_sem);
	init_waitqueue_head(&tty->write_wait);
	init_waitqueue_head(&tty->read_wait);
	INIT_WORK(&tty->hangup_work, do_tty_hangup);
	mutex_init(&tty->atomic_write_lock);
	spin_lock_init(&tty->ctrl_lock);
	spin_lock_init(&tty->flow_lock);
	INIT_LIST_HEAD(&tty->tty_files);
	INIT_WORK(&tty->SAK_work, do_SAK_work);

	tty->driver = driver;
	tty->ops = driver->ops;
	tty->index = idx;
	tty_line_name(driver, idx, tty->name);
	tty->dev = tty_get_device(tty);

	return tty;
}
