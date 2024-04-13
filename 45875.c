const cmsDICTentry* CMSEXPORT cmsDictNextEntry(const cmsDICTentry* e)
{
     if (e == NULL) return NULL;
     return e ->Next;
}
