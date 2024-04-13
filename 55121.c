static int kvm_vm_ioctl_get_pvinfo(struct kvm_ppc_pvinfo *pvinfo)
{
	u32 inst_nop = 0x60000000;
#ifdef CONFIG_KVM_BOOKE_HV
	u32 inst_sc1 = 0x44000022;
	pvinfo->hcall[0] = cpu_to_be32(inst_sc1);
	pvinfo->hcall[1] = cpu_to_be32(inst_nop);
	pvinfo->hcall[2] = cpu_to_be32(inst_nop);
	pvinfo->hcall[3] = cpu_to_be32(inst_nop);
#else
	u32 inst_lis = 0x3c000000;
	u32 inst_ori = 0x60000000;
	u32 inst_sc = 0x44000002;
	u32 inst_imm_mask = 0xffff;

	/*
	 * The hypercall to get into KVM from within guest context is as
	 * follows:
	 *
	 *    lis r0, r0, KVM_SC_MAGIC_R0@h
	 *    ori r0, KVM_SC_MAGIC_R0@l
	 *    sc
	 *    nop
	 */
	pvinfo->hcall[0] = cpu_to_be32(inst_lis | ((KVM_SC_MAGIC_R0 >> 16) & inst_imm_mask));
	pvinfo->hcall[1] = cpu_to_be32(inst_ori | (KVM_SC_MAGIC_R0 & inst_imm_mask));
	pvinfo->hcall[2] = cpu_to_be32(inst_sc);
	pvinfo->hcall[3] = cpu_to_be32(inst_nop);
#endif

	pvinfo->flags = KVM_PPC_PVINFO_FLAGS_EV_IDLE;

	return 0;
}
