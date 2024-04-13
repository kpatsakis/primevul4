static inline bool nested_cpu_has_eptp_switching(struct vmcs12 *vmcs12)
{
	return nested_cpu_has_vmfunc(vmcs12) &&
		(vmcs12->vm_function_control &
		 VMX_VMFUNC_EPTP_SWITCHING);
}
