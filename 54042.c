static struct list_head *cuse_conntbl_head(dev_t devt)
{
	return &cuse_conntbl[(MAJOR(devt) + MINOR(devt)) % CUSE_CONNTBL_LEN];
}
