static void clear_atomic_switch_msr_special(unsigned long entry,
		unsigned long exit)
{
	vmcs_clear_bits(VM_ENTRY_CONTROLS, entry);
	vmcs_clear_bits(VM_EXIT_CONTROLS, exit);
}
