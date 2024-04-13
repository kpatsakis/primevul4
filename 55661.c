static MXFStructuralComponent* mxf_resolve_sourceclip(MXFContext *mxf, UID *strong_ref)
{
    MXFStructuralComponent *component = NULL;

    component = mxf_resolve_strong_ref(mxf, strong_ref, AnyType);
    if (!component)
        return NULL;
    switch (component->type) {
        case SourceClip:
            return component;
        case EssenceGroup:
            return mxf_resolve_essence_group_choice(mxf, (MXFEssenceGroup*) component);
        default:
            break;
    }
    return NULL;
}
