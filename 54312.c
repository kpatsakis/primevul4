static int check_tty_count(struct tty_struct *tty, const char *routine)
{
#ifdef CHECK_TTY_COUNT
	struct list_head *p;
	int count = 0;

	spin_lock(&tty_files_lock);
	list_for_each(p, &tty->tty_files) {
		count++;
	}
	spin_unlock(&tty_files_lock);
	if (tty->driver->type == TTY_DRIVER_TYPE_PTY &&
	    tty->driver->subtype == PTY_TYPE_SLAVE &&
	    tty->link && tty->link->count)
		count++;
	if (tty->count != count) {
		printk(KERN_WARNING "Warning: dev (%s) tty->count(%d) "
				    "!= #fd's(%d) in %s\n",
		       tty->name, tty->count, count, routine);
		return count;
	}
#endif
	return 0;
}
