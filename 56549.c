static void reset_sys_reg_descs(struct kvm_vcpu *vcpu,
			      const struct sys_reg_desc *table, size_t num)
{
	unsigned long i;

	for (i = 0; i < num; i++)
		if (table[i].reset)
			table[i].reset(vcpu, &table[i]);
}
