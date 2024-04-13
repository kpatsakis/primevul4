void proc_clear_tty(struct task_struct *p)
{
	unsigned long flags;
	struct tty_struct *tty;
	spin_lock_irqsave(&p->sighand->siglock, flags);
	tty = p->signal->tty;
	p->signal->tty = NULL;
	spin_unlock_irqrestore(&p->sighand->siglock, flags);
	tty_kref_put(tty);
}
