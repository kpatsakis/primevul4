static sector_t udf_bmap(struct address_space *mapping, sector_t block)
{
	return generic_block_bmap(mapping, block, udf_get_block);
}
