void pin_kill(struct fs_pin *p)
{
	wait_queue_t wait;

	if (!p) {
		rcu_read_unlock();
		return;
	}
	init_wait(&wait);
	spin_lock_irq(&p->wait.lock);
	if (likely(!p->done)) {
		p->done = -1;
		spin_unlock_irq(&p->wait.lock);
		rcu_read_unlock();
		p->kill(p);
		return;
	}
	if (p->done > 0) {
		spin_unlock_irq(&p->wait.lock);
		rcu_read_unlock();
		return;
	}
	__add_wait_queue(&p->wait, &wait);
	while (1) {
		set_current_state(TASK_UNINTERRUPTIBLE);
		spin_unlock_irq(&p->wait.lock);
		rcu_read_unlock();
		schedule();
		rcu_read_lock();
		if (likely(list_empty(&wait.task_list)))
			break;
		/* OK, we know p couldn't have been freed yet */
		spin_lock_irq(&p->wait.lock);
		if (p->done > 0) {
			spin_unlock_irq(&p->wait.lock);
			break;
		}
	}
	rcu_read_unlock();
}
