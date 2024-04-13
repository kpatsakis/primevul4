void pin_insert(struct fs_pin *pin, struct vfsmount *m)
{
	pin_insert_group(pin, m, &m->mnt_sb->s_pins);
}
