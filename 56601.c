static void kvm_cpu_vmxon(u64 addr)
{
	cr4_set_bits(X86_CR4_VMXE);
	intel_pt_handle_vmx(1);

	asm volatile (ASM_VMX_VMXON_RAX
			: : "a"(&addr), "m"(addr)
			: "memory", "cc");
}
