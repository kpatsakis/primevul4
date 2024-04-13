static inline int xstate_sigframe_size(void)
{
	return use_xsave() ? fpu_user_xstate_size + FP_XSTATE_MAGIC2_SIZE :
			fpu_user_xstate_size;
}
