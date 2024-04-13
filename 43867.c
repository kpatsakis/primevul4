static void hardware_disable(void *garbage)
{
	if (vmm_exclusive) {
		vmclear_local_loaded_vmcss();
		kvm_cpu_vmxoff();
	}
	write_cr4(read_cr4() & ~X86_CR4_VMXE);
}
