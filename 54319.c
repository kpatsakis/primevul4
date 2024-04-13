void initialize_tty_struct(struct tty_struct *tty,
		struct tty_driver *driver, int idx)
{
	memset(tty, 0, sizeof(struct tty_struct));
	kref_init(&tty->kref);
	tty->magic = TTY_MAGIC;
	tty_ldisc_init(tty);
	tty->session = NULL;
	tty->pgrp = NULL;
	tty->overrun_time = jiffies;
	tty->buf.head = tty->buf.tail = NULL;
	tty_buffer_init(tty);
	mutex_init(&tty->termios_mutex);
	mutex_init(&tty->ldisc_mutex);
	init_waitqueue_head(&tty->write_wait);
	init_waitqueue_head(&tty->read_wait);
	INIT_WORK(&tty->hangup_work, do_tty_hangup);
	mutex_init(&tty->atomic_read_lock);
	mutex_init(&tty->atomic_write_lock);
	mutex_init(&tty->output_lock);
	mutex_init(&tty->echo_lock);
	spin_lock_init(&tty->read_lock);
	spin_lock_init(&tty->ctrl_lock);
	INIT_LIST_HEAD(&tty->tty_files);
	INIT_WORK(&tty->SAK_work, do_SAK_work);

	tty->driver = driver;
	tty->ops = driver->ops;
	tty->index = idx;
	tty_line_name(driver, idx, tty->name);
	tty->dev = tty_get_device(tty);
}
