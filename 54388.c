kvm_deassign_ioeventfd_idx(struct kvm *kvm, enum kvm_bus bus_idx,
			   struct kvm_ioeventfd *args)
{
	struct _ioeventfd        *p, *tmp;
	struct eventfd_ctx       *eventfd;
	struct kvm_io_bus	 *bus;
	int                       ret = -ENOENT;

	eventfd = eventfd_ctx_fdget(args->fd);
	if (IS_ERR(eventfd))
		return PTR_ERR(eventfd);

	mutex_lock(&kvm->slots_lock);

	list_for_each_entry_safe(p, tmp, &kvm->ioeventfds, list) {
		bool wildcard = !(args->flags & KVM_IOEVENTFD_FLAG_DATAMATCH);

		if (p->bus_idx != bus_idx ||
		    p->eventfd != eventfd  ||
		    p->addr != args->addr  ||
		    p->length != args->len ||
		    p->wildcard != wildcard)
			continue;

		if (!p->wildcard && p->datamatch != args->datamatch)
			continue;

		kvm_io_bus_unregister_dev(kvm, bus_idx, &p->dev);
		bus = kvm_get_bus(kvm, bus_idx);
		if (bus)
			bus->ioeventfd_count--;
		ioeventfd_release(p);
		ret = 0;
		break;
	}

	mutex_unlock(&kvm->slots_lock);

	eventfd_ctx_put(eventfd);

	return ret;
}
