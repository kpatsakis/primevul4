int HTML_put_entity(HTStructured * me, int entity_number)
{
    int nent = (int) HTML_dtd.number_of_entities;

    if (entity_number < nent) {
	HTML_put_string(me, p_entity_values[entity_number]);
	return HT_OK;
    }
    return HT_CANNOT_TRANSLATE;
}
