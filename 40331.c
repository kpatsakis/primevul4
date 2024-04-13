void fb_append_extra_logo(const struct linux_logo *logo, unsigned int n)
{
	if (!n || fb_logo_ex_num == FB_LOGO_EX_NUM_MAX)
		return;

	fb_logo_ex[fb_logo_ex_num].logo = logo;
	fb_logo_ex[fb_logo_ex_num].n = n;
	fb_logo_ex_num++;
}
