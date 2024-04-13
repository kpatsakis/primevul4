static unsigned int ims_pcu_count_fw_records(const struct firmware *fw)
{
	const struct ihex_binrec *rec = (const struct ihex_binrec *)fw->data;
	unsigned int count = 0;

	while (rec) {
		count++;
		rec = ihex_next_binrec(rec);
	}

	return count;
}
