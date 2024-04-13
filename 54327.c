void stop_tty(struct tty_struct *tty)
{
	unsigned long flags;
	spin_lock_irqsave(&tty->ctrl_lock, flags);
	if (tty->stopped) {
		spin_unlock_irqrestore(&tty->ctrl_lock, flags);
		return;
	}
	tty->stopped = 1;
	if (tty->link && tty->link->packet) {
		tty->ctrl_status &= ~TIOCPKT_START;
		tty->ctrl_status |= TIOCPKT_STOP;
		wake_up_interruptible_poll(&tty->link->read_wait, POLLIN);
	}
	spin_unlock_irqrestore(&tty->ctrl_lock, flags);
	if (tty->ops->stop)
		(tty->ops->stop)(tty);
}
