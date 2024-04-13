cmsIntentsList* SearchIntent(cmsUInt32Number Intent)
{
    cmsIntentsList* pt;

    for (pt = Intents; pt != NULL; pt = pt -> Next)
        if (pt ->Intent == Intent) return pt;

    return NULL;
}
