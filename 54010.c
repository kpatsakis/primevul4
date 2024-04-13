int x86_set_memory_region(struct kvm *kvm, int id, gpa_t gpa, u32 size)
{
	int r;

	mutex_lock(&kvm->slots_lock);
	r = __x86_set_memory_region(kvm, id, gpa, size);
	mutex_unlock(&kvm->slots_lock);

	return r;
}
