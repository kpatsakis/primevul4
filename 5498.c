static bool bpf_map_is_rdonly(const struct bpf_map *map)
{
	/* A map is considered read-only if the following condition are true:
	 *
	 * 1) BPF program side cannot change any of the map content. The
	 *    BPF_F_RDONLY_PROG flag is throughout the lifetime of a map
	 *    and was set at map creation time.
	 * 2) The map value(s) have been initialized from user space by a
	 *    loader and then "frozen", such that no new map update/delete
	 *    operations from syscall side are possible for the rest of
	 *    the map's lifetime from that point onwards.
	 * 3) Any parallel/pending map update/delete operations from syscall
	 *    side have been completed. Only after that point, it's safe to
	 *    assume that map value(s) are immutable.
	 */
	return (map->map_flags & BPF_F_RDONLY_PROG) &&
	       READ_ONCE(map->frozen) &&
	       !bpf_map_write_active(map);
}