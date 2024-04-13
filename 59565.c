static void *DestroyProfile(void *profile)
{
  return((void *) DestroyStringInfo((StringInfo *) profile));
}
