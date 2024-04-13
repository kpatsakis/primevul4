static void ldsem_wake(struct ld_semaphore *sem)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&sem->wait_lock, flags);
	__ldsem_wake(sem);
	raw_spin_unlock_irqrestore(&sem->wait_lock, flags);
}
