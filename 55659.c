static MXFStructuralComponent* mxf_resolve_essence_group_choice(MXFContext *mxf, MXFEssenceGroup *essence_group)
{
    MXFStructuralComponent *component = NULL;
    MXFPackage *package = NULL;
    MXFDescriptor *descriptor = NULL;
    int i;

    if (!essence_group || !essence_group->structural_components_count)
        return NULL;

    /* essence groups contains multiple representations of the same media,
       this return the first components with a valid Descriptor typically index 0 */
    for (i =0; i < essence_group->structural_components_count; i++){
        component = mxf_resolve_strong_ref(mxf, &essence_group->structural_components_refs[i], SourceClip);
        if (!component)
            continue;

        if (!(package = mxf_resolve_source_package(mxf, component->source_package_uid)))
            continue;

        descriptor = mxf_resolve_strong_ref(mxf, &package->descriptor_ref, Descriptor);
        if (descriptor)
            return component;
    }
    return NULL;
}
