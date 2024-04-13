HTStructured *HTML_new(HTParentAnchor *anchor,
		       HTFormat format_out,
		       HTStream *stream)
{

    HTStructured *me;

    CTRACE((tfp, "start HTML_new\n"));

    if (format_out != WWW_PLAINTEXT && format_out != WWW_PRESENT) {
	HTStream *intermediate = HTStreamStack(WWW_HTML, format_out,
					       stream, anchor);

	if (intermediate)
	    return HTMLGenerator(intermediate);
	fprintf(stderr, "\n** Internal error: can't parse HTML to %s\n",
		HTAtom_name(format_out));
	exit_immediately(EXIT_FAILURE);
    }

    me = typecalloc(HTStructured);
    if (me == NULL)
	outofmem(__FILE__, "HTML_new");

    /*
     * This used to call 'get_styles()' only on the first time through this
     * function.  However, if the user reloads a page with ^R, the styles[]
     * array is not necessarily the same as it was from 'get_styles()'.  So
     * we reinitialize the whole thing.
     */
    get_styles();

    me->isa = &HTMLPresentation;
    me->node_anchor = anchor;

    me->CurrentA = NULL;
    me->CurrentANum = 0;
    me->base_href = NULL;
    me->map_address = NULL;

    HTChunkInit(&me->title, 128);

    HTChunkInit(&me->object, 128);
    me->object_started = FALSE;
    me->object_declare = FALSE;
    me->object_shapes = FALSE;
    me->object_ismap = FALSE;
    me->object_id = NULL;
    me->object_title = NULL;
    me->object_data = NULL;
    me->object_type = NULL;
    me->object_classid = NULL;
    me->object_codebase = NULL;
    me->object_codetype = NULL;
    me->object_usemap = NULL;
    me->object_name = NULL;

    HTChunkInit(&me->option, 128);
    me->first_option = TRUE;
    me->LastOptionValue = NULL;
    me->LastOptionChecked = FALSE;
    me->select_disabled = FALSE;

    HTChunkInit(&me->textarea, 128);
    me->textarea_name = NULL;
    me->textarea_name_cs = -1;
    me->textarea_accept_cs = NULL;
    me->textarea_cols = 0;
    me->textarea_rows = 4;
    me->textarea_id = NULL;

    HTChunkInit(&me->math, 128);

    HTChunkInit(&me->style_block, 128);

    HTChunkInit(&me->script, 128);

    me->text = 0;
    me->style_change = YES;	/* Force check leading to text creation */
    me->new_style = default_style;
    me->old_style = 0;
    me->current_default_alignment = HT_LEFT;
    me->sp = (me->stack + MAX_NESTING - 1);
    me->skip_stack = 0;
    me->sp->tag_number = -1;	/* INVALID */
    me->sp->style = default_style;	/* INVALID */
    me->sp->style->alignment = HT_LEFT;
    me->stack_overrun = FALSE;

    me->Division_Level = -1;
    me->Underline_Level = 0;
    me->Quote_Level = 0;

    me->UsePlainSpace = FALSE;
    me->HiddenValue = FALSE;
    me->lastraw = -1;

    /*
     * Used for nested lists.  - FM
     */
    me->List_Nesting_Level = -1;	/* counter for list nesting level */
    LYZero_OL_Counter(me);	/* Initializes OL_Counter[] and OL_Type[] */
    me->Last_OL_Count = 0;	/* last count in ordered lists */
    me->Last_OL_Type = '1';	/* last type in ordered lists */

    me->inA = FALSE;
    me->inAPPLET = FALSE;
    me->inAPPLETwithP = FALSE;
    me->inBadBASE = FALSE;
    me->inBadHREF = FALSE;
    me->inBadHTML = FALSE;
    me->inBASE = FALSE;
    me->node_anchor->inBASE = FALSE;
    me->inBoldA = FALSE;
    me->inBoldH = FALSE;
    me->inCAPTION = FALSE;
    me->inCREDIT = FALSE;
    me->inFIG = FALSE;
    me->inFIGwithP = FALSE;
    me->inFONT = FALSE;
    me->inFORM = FALSE;
    me->inLABEL = FALSE;
    me->inP = FALSE;
    me->inPRE = FALSE;
    me->inSELECT = FALSE;
    me->inTABLE = FALSE;
    me->inUnderline = FALSE;

    me->needBoldH = FALSE;

    me->comment_start = NULL;
    me->comment_end = NULL;

#ifdef USE_COLOR_STYLE
#ifdef LY_FIND_LEAKS
    if (Style_className == 0) {
	atexit(free_Style_className);
    }
#endif
    addClassName("", "", (size_t) 0);
    class_string[0] = '\0';
#endif

    /*
     * Create a chartrans stage info structure for the anchor, if it does not
     * exist already (in which case the default MIME stage info will be loaded
     * as well), and load the HTML stage info into me->UCI and me->UCLYhndl.  -
     * FM
     */
    LYGetChartransInfo(me);
    UCTransParams_clear(&me->T);

    /*
     * Load the existing or default input charset info into the holding
     * elements.  We'll believe what is indicated for UCT_STAGE_PARSER.  - FM
     */
    me->inUCLYhndl = HTAnchor_getUCLYhndl(me->node_anchor,
					  UCT_STAGE_PARSER);
    if (me->inUCLYhndl < 0) {
	me->inUCLYhndl = HTAnchor_getUCLYhndl(me->node_anchor,
					      UCT_STAGE_MIME);
	me->inUCI = HTAnchor_getUCInfoStage(me->node_anchor,
					    UCT_STAGE_MIME);
    } else {
	me->inUCI = HTAnchor_getUCInfoStage(me->node_anchor,
					    UCT_STAGE_PARSER);
    }

    /*
     * Load the existing or default output charset info into the holding
     * elements, UCT_STAGE_STRUCTURED should be the same as UCT_STAGE_TEXT at
     * this point, but we could check, perhaps.  - FM
     */
    me->outUCI = HTAnchor_getUCInfoStage(me->node_anchor,
					 UCT_STAGE_STRUCTURED);
    me->outUCLYhndl = HTAnchor_getUCLYhndl(me->node_anchor,
					   UCT_STAGE_STRUCTURED);

    me->target = stream;
    if (stream)
	me->targetClass = *stream->isa;		/* Copy pointers */

    return (HTStructured *) me;
}
