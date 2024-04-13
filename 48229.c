static inline void crypto_check_module_sig(struct module *mod)
{
#ifdef CONFIG_CRYPTO_FIPS
	if (fips_enabled && mod && !mod->sig_ok)
		panic("Module %s signature verification failed in FIPS mode\n",
		      mod->name);
#endif
	return;
}
