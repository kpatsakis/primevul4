static char print_fat_dirty_state(void)
{
    printf("Dirty bit is set. Fs was not properly unmounted and"
	   " some data may be corrupt.\n");

    if (interactive) {
	printf("1) Remove dirty bit\n" "2) No action\n");
	return get_key("12", "?");
    } else
	printf(" Automatically removing dirty bit.\n");
    return '1';
}
