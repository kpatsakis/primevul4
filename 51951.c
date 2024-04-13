void write_label(DOS_FS * fs, char *label)
{
    int l = strlen(label);

    while (l < 11)
	label[l++] = ' ';

    write_boot_label(fs, label);
    write_volume_label(fs, label);
}
