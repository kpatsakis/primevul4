int yr_object_structure_set_member(
    YR_OBJECT* object,
    YR_OBJECT* member)
{
  YR_STRUCTURE_MEMBER* sm;

  assert(object->type == OBJECT_TYPE_STRUCTURE);


  if (yr_object_lookup_field(object,  member->identifier) != NULL)
    return ERROR_DUPLICATED_STRUCTURE_MEMBER;

  sm = (YR_STRUCTURE_MEMBER*) yr_malloc(sizeof(YR_STRUCTURE_MEMBER));

  if (sm == NULL)
    return ERROR_INSUFFICIENT_MEMORY;

  member->parent = object;
  sm->object = member;
  sm->next = object_as_structure(object)->members;

  object_as_structure(object)->members = sm;

  return ERROR_SUCCESS;
}
