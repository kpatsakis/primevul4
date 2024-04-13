static int kvm_is_in_guest(void)
{
	return percpu_read(current_vcpu) != NULL;
}
