HTStyle *LYstyles(int style_number)
{
    if (styles[style_number] == 0)
	get_styles();
    return styles[style_number];
}
