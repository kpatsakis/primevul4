cmsPipeline* _cmsLinkProfiles(cmsContext     ContextID,
                              cmsUInt32Number nProfiles,
                              cmsUInt32Number TheIntents[],
                              cmsHPROFILE     hProfiles[],
                              cmsBool         BPC[],
                              cmsFloat64Number AdaptationStates[],
                              cmsUInt32Number dwFlags)
{
    cmsUInt32Number i;
    cmsIntentsList* Intent;

    if (nProfiles <= 0 || nProfiles > 255) {
         cmsSignalError(ContextID, cmsERROR_RANGE, "Couldn't link '%d' profiles", nProfiles);
        return NULL;
    }

    for (i=0; i < nProfiles; i++) {


        if (TheIntents[i] == INTENT_ABSOLUTE_COLORIMETRIC)
            BPC[i] = FALSE;

        if (TheIntents[i] == INTENT_PERCEPTUAL || TheIntents[i] == INTENT_SATURATION) {

            if (cmsGetProfileVersion(hProfiles[i]) >= 4.0)
                BPC[i] = TRUE;
        }
    }


    Intent = SearchIntent(TheIntents[0]);
    if (Intent == NULL) {
        cmsSignalError(ContextID, cmsERROR_UNKNOWN_EXTENSION, "Unsupported intent '%d'", TheIntents[0]);
        return NULL;
    }

    return Intent ->Link(ContextID, nProfiles, TheIntents, hProfiles, BPC, AdaptationStates, dwFlags);
}
