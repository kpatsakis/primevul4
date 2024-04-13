static void LYStartArea(HTStructured * obj, const char *href,
			const char *alt,
			const char *title,
			int tag_charset)
{
    BOOL new_present[HTML_AREA_ATTRIBUTES];
    const char *new_value[HTML_AREA_ATTRIBUTES];
    int i;

    for (i = 0; i < HTML_AREA_ATTRIBUTES; i++)
	new_present[i] = NO;

    if (alt) {
	new_present[HTML_AREA_ALT] = YES;
	new_value[HTML_AREA_ALT] = (const char *) alt;
    }
    if (non_empty(title)) {
	new_present[HTML_AREA_TITLE] = YES;
	new_value[HTML_AREA_TITLE] = (const char *) title;
    }
    if (href) {
	new_present[HTML_AREA_HREF] = YES;
	new_value[HTML_AREA_HREF] = (const char *) href;
    }

    (*obj->isa->start_element) (obj, HTML_AREA, new_present, new_value,
				tag_charset, 0);
}
