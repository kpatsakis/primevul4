void kvm_arch_hardware_disable(void)
{
	kvm_x86_ops->hardware_disable();
	drop_user_return_notifiers();
}
