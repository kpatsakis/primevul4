static bool mtrr_is_enabled(struct kvm_mtrr *mtrr_state)
{
	return !!(mtrr_state->deftype & IA32_MTRR_DEF_TYPE_E);
}
