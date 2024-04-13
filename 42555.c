static int kvm_vm_ioctl_set_msix_nr(struct kvm *kvm,
				    struct kvm_assigned_msix_nr *entry_nr)
{
	int r = 0;
	struct kvm_assigned_dev_kernel *adev;

	mutex_lock(&kvm->lock);

	adev = kvm_find_assigned_dev(&kvm->arch.assigned_dev_head,
				      entry_nr->assigned_dev_id);
	if (!adev) {
		r = -EINVAL;
		goto msix_nr_out;
	}

	if (adev->entries_nr == 0) {
		adev->entries_nr = entry_nr->entry_nr;
		if (adev->entries_nr == 0 ||
		    adev->entries_nr > KVM_MAX_MSIX_PER_DEV) {
			r = -EINVAL;
			goto msix_nr_out;
		}

		adev->host_msix_entries = kzalloc(sizeof(struct msix_entry) *
						entry_nr->entry_nr,
						GFP_KERNEL);
		if (!adev->host_msix_entries) {
			r = -ENOMEM;
			goto msix_nr_out;
		}
		adev->guest_msix_entries =
			kzalloc(sizeof(struct msix_entry) * entry_nr->entry_nr,
				GFP_KERNEL);
		if (!adev->guest_msix_entries) {
			kfree(adev->host_msix_entries);
			r = -ENOMEM;
			goto msix_nr_out;
		}
	} else /* Not allowed set MSI-X number twice */
		r = -EINVAL;
msix_nr_out:
	mutex_unlock(&kvm->lock);
	return r;
}
