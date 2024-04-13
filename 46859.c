static __exit void hardware_unsetup(void)
{
	free_page((unsigned long)vmx_msr_bitmap_legacy_x2apic);
	free_page((unsigned long)vmx_msr_bitmap_longmode_x2apic);
	free_page((unsigned long)vmx_msr_bitmap_legacy);
	free_page((unsigned long)vmx_msr_bitmap_longmode);
	free_page((unsigned long)vmx_io_bitmap_b);
	free_page((unsigned long)vmx_io_bitmap_a);
	free_page((unsigned long)vmx_vmwrite_bitmap);
	free_page((unsigned long)vmx_vmread_bitmap);
	if (nested)
		free_page((unsigned long)vmx_msr_bitmap_nested);

	free_kvm_area();
}
