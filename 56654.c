static void vmx_handle_external_intr(struct kvm_vcpu *vcpu)
{
	u32 exit_intr_info = vmcs_read32(VM_EXIT_INTR_INFO);
	register void *__sp asm(_ASM_SP);

	if ((exit_intr_info & (INTR_INFO_VALID_MASK | INTR_INFO_INTR_TYPE_MASK))
			== (INTR_INFO_VALID_MASK | INTR_TYPE_EXT_INTR)) {
		unsigned int vector;
		unsigned long entry;
		gate_desc *desc;
		struct vcpu_vmx *vmx = to_vmx(vcpu);
#ifdef CONFIG_X86_64
		unsigned long tmp;
#endif

		vector =  exit_intr_info & INTR_INFO_VECTOR_MASK;
		desc = (gate_desc *)vmx->host_idt_base + vector;
		entry = gate_offset(desc);
		asm volatile(
#ifdef CONFIG_X86_64
			"mov %%" _ASM_SP ", %[sp]\n\t"
			"and $0xfffffffffffffff0, %%" _ASM_SP "\n\t"
			"push $%c[ss]\n\t"
			"push %[sp]\n\t"
#endif
			"pushf\n\t"
			__ASM_SIZE(push) " $%c[cs]\n\t"
			"call *%[entry]\n\t"
			:
#ifdef CONFIG_X86_64
			[sp]"=&r"(tmp),
#endif
			"+r"(__sp)
			:
			[entry]"r"(entry),
			[ss]"i"(__KERNEL_DS),
			[cs]"i"(__KERNEL_CS)
			);
	}
}
