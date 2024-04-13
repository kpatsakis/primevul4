static void init_fsinfo(struct info_sector *i)
{
    i->magic = htole32(0x41615252);
    i->signature = htole32(0x61417272);
    i->free_clusters = htole32(-1);
    i->next_cluster = htole32(2);
    i->boot_sign = htole16(0xaa55);
}
