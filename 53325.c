static void proc_set_tty(struct tty_struct *tty)
{
	spin_lock_irq(&current->sighand->siglock);
	__proc_set_tty(tty);
	spin_unlock_irq(&current->sighand->siglock);
}
