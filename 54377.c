irqfd_ptable_queue_proc(struct file *file, wait_queue_head_t *wqh,
			poll_table *pt)
{
	struct kvm_kernel_irqfd *irqfd =
		container_of(pt, struct kvm_kernel_irqfd, pt);
	add_wait_queue(wqh, &irqfd->wait);
}
