in_upper_context(int depth, int context, xmlNode * xml_node)
{
    if (context == 0) {
        return 0;
    }

    if (xml_node->properties) {
        return depth;

    } else if (depth < context) {
        xmlNode *child = NULL;

        for (child = __xml_first_child(xml_node); child != NULL; child = __xml_next(child)) {
            if (in_upper_context(depth + 1, context, child)) {
                return depth;
            }
        }
    }
    return 0;
}
