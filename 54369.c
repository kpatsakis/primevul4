ioeventfd_check_collision(struct kvm *kvm, struct _ioeventfd *p)
{
	struct _ioeventfd *_p;

	list_for_each_entry(_p, &kvm->ioeventfds, list)
		if (_p->bus_idx == p->bus_idx &&
		    _p->addr == p->addr &&
		    (!_p->length || !p->length ||
		     (_p->length == p->length &&
		      (_p->wildcard || p->wildcard ||
		       _p->datamatch == p->datamatch))))
			return true;

	return false;
}
