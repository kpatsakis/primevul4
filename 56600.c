static void kvm_cpu_vmxoff(void)
{
	asm volatile (__ex(ASM_VMX_VMXOFF) : : : "cc");

	intel_pt_handle_vmx(0);
	cr4_clear_bits(X86_CR4_VMXE);
}
