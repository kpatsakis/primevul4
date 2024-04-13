static char *MakeNewImageValue(STRING2PTR value)
{
    char *ptr;
    char *newtitle = NULL;

    StrAllocCopy(newtitle, "[");
    ptr = (value[HTML_INPUT_SRC]
	   ? strrchr(value[HTML_INPUT_SRC], '/')
	   : 0);
    if (!ptr) {
	StrAllocCat(newtitle, value[HTML_INPUT_SRC]);
    } else {
	StrAllocCat(newtitle, ptr + 1);
    }
    StrAllocCat(newtitle, "]-Submit");
    return newtitle;
}
