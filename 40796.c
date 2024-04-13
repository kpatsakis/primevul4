struct sighand_struct *__lock_task_sighand(struct task_struct *tsk,
					   unsigned long *flags)
{
	struct sighand_struct *sighand;

	for (;;) {
		local_irq_save(*flags);
		rcu_read_lock();
		sighand = rcu_dereference(tsk->sighand);
		if (unlikely(sighand == NULL)) {
			rcu_read_unlock();
			local_irq_restore(*flags);
			break;
		}

		spin_lock(&sighand->siglock);
		if (likely(sighand == tsk->sighand)) {
			rcu_read_unlock();
			break;
		}
		spin_unlock(&sighand->siglock);
		rcu_read_unlock();
		local_irq_restore(*flags);
	}

	return sighand;
}
