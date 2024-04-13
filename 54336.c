int tty_do_resize(struct tty_struct *tty, struct winsize *ws)
{
	struct pid *pgrp;
	unsigned long flags;

	/* Lock the tty */
	mutex_lock(&tty->termios_mutex);
	if (!memcmp(ws, &tty->winsize, sizeof(*ws)))
		goto done;
	/* Get the PID values and reference them so we can
	   avoid holding the tty ctrl lock while sending signals */
	spin_lock_irqsave(&tty->ctrl_lock, flags);
	pgrp = get_pid(tty->pgrp);
	spin_unlock_irqrestore(&tty->ctrl_lock, flags);

	if (pgrp)
		kill_pgrp(pgrp, SIGWINCH, 1);
	put_pid(pgrp);

	tty->winsize = *ws;
done:
	mutex_unlock(&tty->termios_mutex);
	return 0;
}
