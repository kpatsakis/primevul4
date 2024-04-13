int tty_do_resize(struct tty_struct *tty, struct winsize *ws)
{
	struct pid *pgrp;

	/* Lock the tty */
	mutex_lock(&tty->winsize_mutex);
	if (!memcmp(ws, &tty->winsize, sizeof(*ws)))
		goto done;

	/* Signal the foreground process group */
	pgrp = tty_get_pgrp(tty);
	if (pgrp)
		kill_pgrp(pgrp, SIGWINCH, 1);
	put_pid(pgrp);

	tty->winsize = *ws;
done:
	mutex_unlock(&tty->winsize_mutex);
	return 0;
}
