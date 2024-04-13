static void init_vmcs_shadow_fields(void)
{
	int i, j;

	/* No checks for read only fields yet */

	for (i = j = 0; i < max_shadow_read_write_fields; i++) {
		switch (shadow_read_write_fields[i]) {
		case GUEST_BNDCFGS:
			if (!kvm_mpx_supported())
				continue;
			break;
		default:
			break;
		}

		if (j < i)
			shadow_read_write_fields[j] =
				shadow_read_write_fields[i];
		j++;
	}
	max_shadow_read_write_fields = j;

	/* shadowed fields guest access without vmexit */
	for (i = 0; i < max_shadow_read_write_fields; i++) {
		unsigned long field = shadow_read_write_fields[i];

		clear_bit(field, vmx_vmwrite_bitmap);
		clear_bit(field, vmx_vmread_bitmap);
		if (vmcs_field_type(field) == VMCS_FIELD_TYPE_U64) {
			clear_bit(field + 1, vmx_vmwrite_bitmap);
			clear_bit(field + 1, vmx_vmread_bitmap);
		}
	}
	for (i = 0; i < max_shadow_read_only_fields; i++) {
		unsigned long field = shadow_read_only_fields[i];

		clear_bit(field, vmx_vmread_bitmap);
		if (vmcs_field_type(field) == VMCS_FIELD_TYPE_U64)
			clear_bit(field + 1, vmx_vmread_bitmap);
	}
}
