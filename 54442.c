static void clear_objectdata(HTStructured * me)
{
    if (me) {
	HTChunkClear(&me->object);
	me->object_started = FALSE;
	me->object_declare = FALSE;
	me->object_shapes = FALSE;
	me->object_ismap = FALSE;
	FREE(me->object_usemap);
	FREE(me->object_id);
	FREE(me->object_title);
	FREE(me->object_data);
	FREE(me->object_type);
	FREE(me->object_classid);
	FREE(me->object_codebase);
	FREE(me->object_codetype);
	FREE(me->object_name);
    }
}
