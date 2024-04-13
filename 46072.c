static unsigned long kvm_get_guest_ip(void)
{
	unsigned long ip = 0;

	if (percpu_read(current_vcpu))
		ip = kvm_rip_read(percpu_read(current_vcpu));

	return ip;
}
