static void change_paragraph_style(HTStructured * me, HTStyle *style)
{
    if (me->new_style != style) {
	me->style_change = YES;
	me->new_style = style;
    }
    me->in_word = NO;
}
