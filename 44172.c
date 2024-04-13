static int cm_compare_data(struct ib_cm_compare_data *src_data,
			   struct ib_cm_compare_data *dst_data)
{
	u8 src[IB_CM_COMPARE_SIZE];
	u8 dst[IB_CM_COMPARE_SIZE];

	if (!src_data || !dst_data)
		return 0;

	cm_mask_copy(src, src_data->data, dst_data->mask);
	cm_mask_copy(dst, dst_data->data, src_data->mask);
	return memcmp(src, dst, IB_CM_COMPARE_SIZE);
}
