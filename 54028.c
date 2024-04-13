static void __ldsem_wake_writer(struct ld_semaphore *sem)
{
	struct ldsem_waiter *waiter;

	waiter = list_entry(sem->write_wait.next, struct ldsem_waiter, list);
	wake_up_process(waiter->task);
}
