static bool vmx_has_high_real_mode_segbase(void)
{
	return enable_unrestricted_guest || emulate_invalid_guest_state;
}
