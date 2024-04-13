static int mmu_shrink(struct shrinker *shrink, struct shrink_control *sc)
{
	struct kvm *kvm;
	int nr_to_scan = sc->nr_to_scan;

	if (nr_to_scan == 0)
		goto out;

	raw_spin_lock(&kvm_lock);

	list_for_each_entry(kvm, &vm_list, vm_list) {
		int idx;
		LIST_HEAD(invalid_list);

		/*
		 * Never scan more than sc->nr_to_scan VM instances.
		 * Will not hit this condition practically since we do not try
		 * to shrink more than one VM and it is very unlikely to see
		 * !n_used_mmu_pages so many times.
		 */
		if (!nr_to_scan--)
			break;
		/*
		 * n_used_mmu_pages is accessed without holding kvm->mmu_lock
		 * here. We may skip a VM instance errorneosly, but we do not
		 * want to shrink a VM that only started to populate its MMU
		 * anyway.
		 */
		if (!kvm->arch.n_used_mmu_pages &&
		      !kvm_has_zapped_obsolete_pages(kvm))
			continue;

		idx = srcu_read_lock(&kvm->srcu);
		spin_lock(&kvm->mmu_lock);

		if (kvm_has_zapped_obsolete_pages(kvm)) {
			kvm_mmu_commit_zap_page(kvm,
			      &kvm->arch.zapped_obsolete_pages);
			goto unlock;
		}

		prepare_zap_oldest_mmu_page(kvm, &invalid_list);
		kvm_mmu_commit_zap_page(kvm, &invalid_list);

unlock:
		spin_unlock(&kvm->mmu_lock);
		srcu_read_unlock(&kvm->srcu, idx);

		list_move_tail(&kvm->vm_list, &vm_list);
		break;
	}

	raw_spin_unlock(&kvm_lock);

out:
	return percpu_counter_read_positive(&kvm_total_used_mmu_pages);
}
