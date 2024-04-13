static u64 calc_stripe_length(u64 type, u64 chunk_len, int num_stripes)
{
	const int data_stripes = calc_data_stripes(type, num_stripes);

	return div_u64(chunk_len, data_stripes);
}