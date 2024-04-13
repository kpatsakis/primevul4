void __do_SAK(struct tty_struct *tty)
{
#ifdef TTY_SOFT_SAK
	tty_hangup(tty);
#else
	struct task_struct *g, *p;
	struct pid *session;
	int		i;
	struct file	*filp;
	struct fdtable *fdt;

	if (!tty)
		return;
	session = tty->session;

	tty_ldisc_flush(tty);

	tty_driver_flush_buffer(tty);

	read_lock(&tasklist_lock);
	/* Kill the entire session */
	do_each_pid_task(session, PIDTYPE_SID, p) {
		printk(KERN_NOTICE "SAK: killed process %d"
			" (%s): task_session(p)==tty->session\n",
			task_pid_nr(p), p->comm);
		send_sig(SIGKILL, p, 1);
	} while_each_pid_task(session, PIDTYPE_SID, p);
	/* Now kill any processes that happen to have the
	 * tty open.
	 */
	do_each_thread(g, p) {
		if (p->signal->tty == tty) {
			printk(KERN_NOTICE "SAK: killed process %d"
			    " (%s): task_session(p)==tty->session\n",
			    task_pid_nr(p), p->comm);
			send_sig(SIGKILL, p, 1);
			continue;
		}
		task_lock(p);
		if (p->files) {
			/*
			 * We don't take a ref to the file, so we must
			 * hold ->file_lock instead.
			 */
			spin_lock(&p->files->file_lock);
			fdt = files_fdtable(p->files);
			for (i = 0; i < fdt->max_fds; i++) {
				filp = fcheck_files(p->files, i);
				if (!filp)
					continue;
				if (filp->f_op->read == tty_read &&
				    file_tty(filp) == tty) {
					printk(KERN_NOTICE "SAK: killed process %d"
					    " (%s): fd#%d opened to the tty\n",
					    task_pid_nr(p), p->comm, i);
					force_sig(SIGKILL, p);
					break;
				}
			}
			spin_unlock(&p->files->file_lock);
		}
		task_unlock(p);
	} while_each_thread(g, p);
	read_unlock(&tasklist_lock);
#endif
}
