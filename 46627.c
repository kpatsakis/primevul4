static int md_congested(void *data, int bits)
{
	struct mddev *mddev = data;
	return mddev_congested(mddev, bits);
}
