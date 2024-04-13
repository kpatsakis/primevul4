static bool nested_ept_ad_enabled(struct kvm_vcpu *vcpu)
{
	return nested_ept_get_cr3(vcpu) & VMX_EPTP_AD_ENABLE_BIT;
}
