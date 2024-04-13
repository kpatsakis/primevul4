static int kvm_vm_ioctl_set_msix_entry(struct kvm *kvm,
				       struct kvm_assigned_msix_entry *entry)
{
	int r = 0, i;
	struct kvm_assigned_dev_kernel *adev;

	mutex_lock(&kvm->lock);

	adev = kvm_find_assigned_dev(&kvm->arch.assigned_dev_head,
				      entry->assigned_dev_id);

	if (!adev) {
		r = -EINVAL;
		goto msix_entry_out;
	}

	for (i = 0; i < adev->entries_nr; i++)
		if (adev->guest_msix_entries[i].vector == 0 ||
		    adev->guest_msix_entries[i].entry == entry->entry) {
			adev->guest_msix_entries[i].entry = entry->entry;
			adev->guest_msix_entries[i].vector = entry->gsi;
			adev->host_msix_entries[i].entry = entry->entry;
			break;
		}
	if (i == adev->entries_nr) {
		r = -ENOSPC;
		goto msix_entry_out;
	}

msix_entry_out:
	mutex_unlock(&kvm->lock);

	return r;
}
