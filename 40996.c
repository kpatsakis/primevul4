static void perf_mmap_close(struct vm_area_struct *vma)
{
	struct perf_event *event = vma->vm_file->private_data;

	if (atomic_dec_and_mutex_lock(&event->mmap_count, &event->mmap_mutex)) {
		unsigned long size = perf_data_size(event->rb);
		struct user_struct *user = event->mmap_user;
		struct ring_buffer *rb = event->rb;

		atomic_long_sub((size >> PAGE_SHIFT) + 1, &user->locked_vm);
		vma->vm_mm->pinned_vm -= event->mmap_locked;
		rcu_assign_pointer(event->rb, NULL);
		ring_buffer_detach(event, rb);
		mutex_unlock(&event->mmap_mutex);

		ring_buffer_put(rb);
		free_uid(user);
	}
}
