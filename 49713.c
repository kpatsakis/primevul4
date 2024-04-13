static inline void LOAD_OP(int I, u64 *W, const u8 *input)
{
	W[I] = get_unaligned_be64((__u64 *)input + I);
}
