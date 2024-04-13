static u8 mtrr_default_type(struct kvm_mtrr *mtrr_state)
{
	return mtrr_state->deftype & IA32_MTRR_DEF_TYPE_TYPE_MASK;
}
