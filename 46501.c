static Sg_device *sg_lookup_dev(int dev)
{
	return idr_find(&sg_index_idr, dev);
}
