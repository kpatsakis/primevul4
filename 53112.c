static void __init evm_init_config(void)
{
#ifdef CONFIG_EVM_ATTR_FSUUID
	evm_hmac_attrs |= EVM_ATTR_FSUUID;
#endif
	pr_info("HMAC attrs: 0x%x\n", evm_hmac_attrs);
}
