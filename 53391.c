static int tty_signal_session_leader(struct tty_struct *tty, int exit_session)
{
	struct task_struct *p;
	int refs = 0;
	struct pid *tty_pgrp = NULL;

	read_lock(&tasklist_lock);
	if (tty->session) {
		do_each_pid_task(tty->session, PIDTYPE_SID, p) {
			spin_lock_irq(&p->sighand->siglock);
			if (p->signal->tty == tty) {
				p->signal->tty = NULL;
				/* We defer the dereferences outside fo
				   the tasklist lock */
				refs++;
			}
			if (!p->signal->leader) {
				spin_unlock_irq(&p->sighand->siglock);
				continue;
			}
			__group_send_sig_info(SIGHUP, SEND_SIG_PRIV, p);
			__group_send_sig_info(SIGCONT, SEND_SIG_PRIV, p);
			put_pid(p->signal->tty_old_pgrp);  /* A noop */
			spin_lock(&tty->ctrl_lock);
			tty_pgrp = get_pid(tty->pgrp);
			if (tty->pgrp)
				p->signal->tty_old_pgrp = get_pid(tty->pgrp);
			spin_unlock(&tty->ctrl_lock);
			spin_unlock_irq(&p->sighand->siglock);
		} while_each_pid_task(tty->session, PIDTYPE_SID, p);
	}
	read_unlock(&tasklist_lock);

	if (tty_pgrp) {
		if (exit_session)
			kill_pgrp(tty_pgrp, SIGHUP, exit_session);
		put_pid(tty_pgrp);
	}

	return refs;
}
