static void vmcs_set_secondary_exec_control(u32 new_ctl)
{
	/*
	 * These bits in the secondary execution controls field
	 * are dynamic, the others are mostly based on the hypervisor
	 * architecture and the guest's CPUID.  Do not touch the
	 * dynamic bits.
	 */
	u32 mask =
		SECONDARY_EXEC_SHADOW_VMCS |
		SECONDARY_EXEC_VIRTUALIZE_X2APIC_MODE |
		SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES;

	u32 cur_ctl = vmcs_read32(SECONDARY_VM_EXEC_CONTROL);

	vmcs_write32(SECONDARY_VM_EXEC_CONTROL,
		     (new_ctl & ~mask) | (cur_ctl & mask));
}
