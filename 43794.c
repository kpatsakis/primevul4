 static gpa_t nonpaging_gva_to_gpa(struct kvm_vcpu *vcpu, gva_t vaddr,
 				  u32 access, struct x86_exception *exception)
{
	if (exception)
		exception->error_code = 0;
	return vaddr;
}
