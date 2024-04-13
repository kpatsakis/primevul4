static void copy_shadow_to_vmcs12(struct vcpu_vmx *vmx)
{
	int i;
	unsigned long field;
	u64 field_value;
	struct vmcs *shadow_vmcs = vmx->nested.current_shadow_vmcs;
	const unsigned long *fields = shadow_read_write_fields;
	const int num_fields = max_shadow_read_write_fields;

	preempt_disable();

	vmcs_load(shadow_vmcs);

	for (i = 0; i < num_fields; i++) {
		field = fields[i];
		switch (vmcs_field_type(field)) {
		case VMCS_FIELD_TYPE_U16:
			field_value = vmcs_read16(field);
			break;
		case VMCS_FIELD_TYPE_U32:
			field_value = vmcs_read32(field);
			break;
		case VMCS_FIELD_TYPE_U64:
			field_value = vmcs_read64(field);
			break;
		case VMCS_FIELD_TYPE_NATURAL_WIDTH:
			field_value = vmcs_readl(field);
			break;
		default:
			WARN_ON(1);
			continue;
		}
		vmcs12_write_any(&vmx->vcpu, field, field_value);
	}

	vmcs_clear(shadow_vmcs);
	vmcs_load(vmx->loaded_vmcs->vmcs);

	preempt_enable();
}
