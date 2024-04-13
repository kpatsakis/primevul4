static void hardware_disable(void)
{
	vmclear_local_loaded_vmcss();
	kvm_cpu_vmxoff();
}
