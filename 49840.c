static inline u32 uint8p_to_uint32_be(u8 *in)
{
	u32 *data = (u32 *)in;

	return cpu_to_be32p(data);
}
