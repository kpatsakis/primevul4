static void check_fat_state_bit(DOS_FS * fs, void *b)
{
    if (fs->fat_bits == 32) {
	struct boot_sector *b32 = b;

	if (b32->reserved3 & FAT_STATE_DIRTY) {
	    printf("0x41: ");
	    if (print_fat_dirty_state() == '1') {
		b32->reserved3 &= ~FAT_STATE_DIRTY;
		fs_write(0, sizeof(*b32), b32);
	    }
	}
    } else {
	struct boot_sector_16 *b16 = b;

	if (b16->reserved2 & FAT_STATE_DIRTY) {
	    printf("0x25: ");
	    if (print_fat_dirty_state() == '1') {
		b16->reserved2 &= ~FAT_STATE_DIRTY;
		fs_write(0, sizeof(*b16), b16);
	    }
	}
    }
}
