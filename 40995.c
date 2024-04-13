static int perf_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct perf_event *event = file->private_data;
	unsigned long user_locked, user_lock_limit;
	struct user_struct *user = current_user();
	unsigned long locked, lock_limit;
	struct ring_buffer *rb;
	unsigned long vma_size;
	unsigned long nr_pages;
	long user_extra, extra;
	int ret = 0, flags = 0;

	/*
	 * Don't allow mmap() of inherited per-task counters. This would
	 * create a performance issue due to all children writing to the
	 * same rb.
	 */
	if (event->cpu == -1 && event->attr.inherit)
		return -EINVAL;

	if (!(vma->vm_flags & VM_SHARED))
		return -EINVAL;

	vma_size = vma->vm_end - vma->vm_start;
	nr_pages = (vma_size / PAGE_SIZE) - 1;

	/*
	 * If we have rb pages ensure they're a power-of-two number, so we
	 * can do bitmasks instead of modulo.
	 */
	if (nr_pages != 0 && !is_power_of_2(nr_pages))
		return -EINVAL;

	if (vma_size != PAGE_SIZE * (1 + nr_pages))
		return -EINVAL;

	if (vma->vm_pgoff != 0)
		return -EINVAL;

	WARN_ON_ONCE(event->ctx->parent_ctx);
	mutex_lock(&event->mmap_mutex);
	if (event->rb) {
		if (event->rb->nr_pages == nr_pages)
			atomic_inc(&event->rb->refcount);
		else
			ret = -EINVAL;
		goto unlock;
	}

	user_extra = nr_pages + 1;
	user_lock_limit = sysctl_perf_event_mlock >> (PAGE_SHIFT - 10);

	/*
	 * Increase the limit linearly with more CPUs:
	 */
	user_lock_limit *= num_online_cpus();

	user_locked = atomic_long_read(&user->locked_vm) + user_extra;

	extra = 0;
	if (user_locked > user_lock_limit)
		extra = user_locked - user_lock_limit;

	lock_limit = rlimit(RLIMIT_MEMLOCK);
	lock_limit >>= PAGE_SHIFT;
	locked = vma->vm_mm->pinned_vm + extra;

	if ((locked > lock_limit) && perf_paranoid_tracepoint_raw() &&
		!capable(CAP_IPC_LOCK)) {
		ret = -EPERM;
		goto unlock;
	}

	WARN_ON(event->rb);

	if (vma->vm_flags & VM_WRITE)
		flags |= RING_BUFFER_WRITABLE;

	rb = rb_alloc(nr_pages, 
		event->attr.watermark ? event->attr.wakeup_watermark : 0,
		event->cpu, flags);

	if (!rb) {
		ret = -ENOMEM;
		goto unlock;
	}
	rcu_assign_pointer(event->rb, rb);

	atomic_long_add(user_extra, &user->locked_vm);
	event->mmap_locked = extra;
	event->mmap_user = get_current_user();
	vma->vm_mm->pinned_vm += event->mmap_locked;

	perf_event_update_userpage(event);

unlock:
	if (!ret)
		atomic_inc(&event->mmap_count);
	mutex_unlock(&event->mmap_mutex);

	vma->vm_flags |= VM_DONTEXPAND | VM_DONTDUMP;
	vma->vm_ops = &perf_mmap_vmops;

	return ret;
}
