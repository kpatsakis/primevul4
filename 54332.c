int tty_check_change(struct tty_struct *tty)
{
	unsigned long flags;
	int ret = 0;

	if (current->signal->tty != tty)
		return 0;

	spin_lock_irqsave(&tty->ctrl_lock, flags);

	if (!tty->pgrp) {
		printk(KERN_WARNING "tty_check_change: tty->pgrp == NULL!\n");
		goto out_unlock;
	}
	if (task_pgrp(current) == tty->pgrp)
		goto out_unlock;
	spin_unlock_irqrestore(&tty->ctrl_lock, flags);
	if (is_ignored(SIGTTOU))
		goto out;
	if (is_current_pgrp_orphaned()) {
		ret = -EIO;
		goto out;
	}
	kill_pgrp(task_pgrp(current), SIGTTOU, 1);
	set_thread_flag(TIF_SIGPENDING);
	ret = -ERESTARTSYS;
out:
	return ret;
out_unlock:
	spin_unlock_irqrestore(&tty->ctrl_lock, flags);
	return ret;
}
