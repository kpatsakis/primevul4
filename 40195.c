static inline void put_binfmt(struct linux_binfmt * fmt)
{
	module_put(fmt->module);
}
