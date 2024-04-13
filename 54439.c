void actually_set_style(HTStructured * me)
{
    if (!me->text) {		/* First time through */
	LYGetChartransInfo(me);
	UCSetTransParams(&me->T,
			 me->UCLYhndl, me->UCI,
			 HTAnchor_getUCLYhndl(me->node_anchor,
					      UCT_STAGE_HTEXT),
			 HTAnchor_getUCInfoStage(me->node_anchor,
						 UCT_STAGE_HTEXT));
	me->text = HText_new2(me->node_anchor, me->target);
	HText_beginAppend(me->text);
	HText_setStyle(me->text, me->new_style);
	me->in_word = NO;
	LYCheckForContentBase(me);
    } else {
	HText_setStyle(me->text, me->new_style);
    }

    me->old_style = me->new_style;
    me->style_change = NO;
}
