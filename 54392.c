kvm_irqfd_assign(struct kvm *kvm, struct kvm_irqfd *args)
{
	struct kvm_kernel_irqfd *irqfd, *tmp;
	struct fd f;
	struct eventfd_ctx *eventfd = NULL, *resamplefd = NULL;
	int ret;
	unsigned int events;
	int idx;

	if (!kvm_arch_intc_initialized(kvm))
		return -EAGAIN;

	irqfd = kzalloc(sizeof(*irqfd), GFP_KERNEL);
	if (!irqfd)
		return -ENOMEM;

	irqfd->kvm = kvm;
	irqfd->gsi = args->gsi;
	INIT_LIST_HEAD(&irqfd->list);
	INIT_WORK(&irqfd->inject, irqfd_inject);
	INIT_WORK(&irqfd->shutdown, irqfd_shutdown);
	seqcount_init(&irqfd->irq_entry_sc);

	f = fdget(args->fd);
	if (!f.file) {
		ret = -EBADF;
		goto out;
	}

	eventfd = eventfd_ctx_fileget(f.file);
	if (IS_ERR(eventfd)) {
		ret = PTR_ERR(eventfd);
		goto fail;
	}

	irqfd->eventfd = eventfd;

	if (args->flags & KVM_IRQFD_FLAG_RESAMPLE) {
		struct kvm_kernel_irqfd_resampler *resampler;

		resamplefd = eventfd_ctx_fdget(args->resamplefd);
		if (IS_ERR(resamplefd)) {
			ret = PTR_ERR(resamplefd);
			goto fail;
		}

		irqfd->resamplefd = resamplefd;
		INIT_LIST_HEAD(&irqfd->resampler_link);

		mutex_lock(&kvm->irqfds.resampler_lock);

		list_for_each_entry(resampler,
				    &kvm->irqfds.resampler_list, link) {
			if (resampler->notifier.gsi == irqfd->gsi) {
				irqfd->resampler = resampler;
				break;
			}
		}

		if (!irqfd->resampler) {
			resampler = kzalloc(sizeof(*resampler), GFP_KERNEL);
			if (!resampler) {
				ret = -ENOMEM;
				mutex_unlock(&kvm->irqfds.resampler_lock);
				goto fail;
			}

			resampler->kvm = kvm;
			INIT_LIST_HEAD(&resampler->list);
			resampler->notifier.gsi = irqfd->gsi;
			resampler->notifier.irq_acked = irqfd_resampler_ack;
			INIT_LIST_HEAD(&resampler->link);

			list_add(&resampler->link, &kvm->irqfds.resampler_list);
			kvm_register_irq_ack_notifier(kvm,
						      &resampler->notifier);
			irqfd->resampler = resampler;
		}

		list_add_rcu(&irqfd->resampler_link, &irqfd->resampler->list);
		synchronize_srcu(&kvm->irq_srcu);

		mutex_unlock(&kvm->irqfds.resampler_lock);
	}

	/*
	 * Install our own custom wake-up handling so we are notified via
	 * a callback whenever someone signals the underlying eventfd
	 */
	init_waitqueue_func_entry(&irqfd->wait, irqfd_wakeup);
	init_poll_funcptr(&irqfd->pt, irqfd_ptable_queue_proc);

	spin_lock_irq(&kvm->irqfds.lock);

	ret = 0;
	list_for_each_entry(tmp, &kvm->irqfds.items, list) {
		if (irqfd->eventfd != tmp->eventfd)
			continue;
		/* This fd is used for another irq already. */
		ret = -EBUSY;
		spin_unlock_irq(&kvm->irqfds.lock);
		goto fail;
	}

	idx = srcu_read_lock(&kvm->irq_srcu);
	irqfd_update(kvm, irqfd);
	srcu_read_unlock(&kvm->irq_srcu, idx);

	list_add_tail(&irqfd->list, &kvm->irqfds.items);

	spin_unlock_irq(&kvm->irqfds.lock);

	/*
	 * Check if there was an event already pending on the eventfd
	 * before we registered, and trigger it as if we didn't miss it.
	 */
	events = f.file->f_op->poll(f.file, &irqfd->pt);

	if (events & POLLIN)
		schedule_work(&irqfd->inject);

	/*
	 * do not drop the file until the irqfd is fully initialized, otherwise
	 * we might race against the POLLHUP
	 */
	fdput(f);
#ifdef CONFIG_HAVE_KVM_IRQ_BYPASS
	if (kvm_arch_has_irq_bypass()) {
		irqfd->consumer.token = (void *)irqfd->eventfd;
		irqfd->consumer.add_producer = kvm_arch_irq_bypass_add_producer;
		irqfd->consumer.del_producer = kvm_arch_irq_bypass_del_producer;
		irqfd->consumer.stop = kvm_arch_irq_bypass_stop;
		irqfd->consumer.start = kvm_arch_irq_bypass_start;
		ret = irq_bypass_register_consumer(&irqfd->consumer);
		if (ret)
			pr_info("irq bypass consumer (token %p) registration fails: %d\n",
				irqfd->consumer.token, ret);
	}
#endif

	return 0;

fail:
	if (irqfd->resampler)
		irqfd_resampler_shutdown(irqfd);

	if (resamplefd && !IS_ERR(resamplefd))
		eventfd_ctx_put(resamplefd);

	if (eventfd && !IS_ERR(eventfd))
		eventfd_ctx_put(eventfd);

	fdput(f);

out:
	kfree(irqfd);
	return ret;
}
