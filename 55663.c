static MXFTimecodeComponent* mxf_resolve_timecode_component(MXFContext *mxf, UID *strong_ref)
{
    MXFStructuralComponent *component = NULL;
    MXFPulldownComponent *pulldown = NULL;

    component = mxf_resolve_strong_ref(mxf, strong_ref, AnyType);
    if (!component)
        return NULL;

    switch (component->type) {
    case TimecodeComponent:
        return (MXFTimecodeComponent*)component;
    case PulldownComponent: /* timcode component may be located on a pulldown component */
        pulldown = (MXFPulldownComponent*)component;
        return mxf_resolve_strong_ref(mxf, &pulldown->input_segment_ref, TimecodeComponent);
    default:
        break;
    }
    return NULL;
}
