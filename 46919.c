static void vmx_flush_log_dirty(struct kvm *kvm)
{
	kvm_flush_pml_buffers(kvm);
}
