struct tty_struct *tty_init_dev(struct tty_driver *driver, int idx)
{
	struct tty_struct *tty;
	int retval;

	/*
	 * First time open is complex, especially for PTY devices.
	 * This code guarantees that either everything succeeds and the
	 * TTY is ready for operation, or else the table slots are vacated
	 * and the allocated memory released.  (Except that the termios
	 * and locked termios may be retained.)
	 */

	if (!try_module_get(driver->owner))
		return ERR_PTR(-ENODEV);

	tty = alloc_tty_struct(driver, idx);
	if (!tty) {
		retval = -ENOMEM;
		goto err_module_put;
	}

	tty_lock(tty);
	retval = tty_driver_install_tty(driver, tty);
	if (retval < 0)
		goto err_deinit_tty;

	if (!tty->port)
		tty->port = driver->ports[idx];

	WARN_RATELIMIT(!tty->port,
			"%s: %s driver does not set tty->port. This will crash the kernel later. Fix the driver!\n",
			__func__, tty->driver->name);

	tty->port->itty = tty;

	/*
	 * Structures all installed ... call the ldisc open routines.
	 * If we fail here just call release_tty to clean up.  No need
	 * to decrement the use counts, as release_tty doesn't care.
	 */
	retval = tty_ldisc_setup(tty, tty->link);
	if (retval)
		goto err_release_tty;
	/* Return the tty locked so that it cannot vanish under the caller */
	return tty;

err_deinit_tty:
	tty_unlock(tty);
	deinitialize_tty_struct(tty);
	free_tty_struct(tty);
err_module_put:
	module_put(driver->owner);
	return ERR_PTR(retval);

	/* call the tty release_tty routine to clean out this slot */
err_release_tty:
	tty_unlock(tty);
	tty_info_ratelimited(tty, "ldisc open failed (%d), clearing slot %d\n",
			     retval, idx);
	release_tty(tty, idx);
	return ERR_PTR(retval);
}
