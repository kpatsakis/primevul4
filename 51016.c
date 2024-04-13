static inline struct f_fs_opts *to_ffs_opts(struct config_item *item)
{
	return container_of(to_config_group(item), struct f_fs_opts,
			    func_inst.group);
}
