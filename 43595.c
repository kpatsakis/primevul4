static struct se_device *rd_alloc_device(struct se_hba *hba, const char *name)
{
	struct rd_dev *rd_dev;
	struct rd_host *rd_host = hba->hba_ptr;

	rd_dev = kzalloc(sizeof(struct rd_dev), GFP_KERNEL);
	if (!rd_dev) {
		pr_err("Unable to allocate memory for struct rd_dev\n");
		return NULL;
	}

	rd_dev->rd_host = rd_host;

	return &rd_dev->dev;
}
