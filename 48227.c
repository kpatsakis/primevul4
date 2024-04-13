int crypto_attr_u32(struct rtattr *rta, u32 *num)
{
	struct crypto_attr_u32 *nu32;

	if (!rta)
		return -ENOENT;
	if (RTA_PAYLOAD(rta) < sizeof(*nu32))
		return -EINVAL;
	if (rta->rta_type != CRYPTOA_U32)
		return -EINVAL;

	nu32 = RTA_DATA(rta);
	*num = nu32->num;

	return 0;
}
