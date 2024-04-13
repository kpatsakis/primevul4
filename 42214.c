void vhost_enable_zcopy(int vq)
{
	vhost_zcopy_mask |= 0x1 << vq;
}
