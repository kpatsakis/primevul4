v8::Handle<v8::Value> V8ThrowException::throwDOMException(int ec, const String& sanitizedMessage, const String& unsanitizedMessage, const v8::Handle<v8::Object>& creationContext, v8::Isolate* isolate)
{
    ASSERT(ec == SecurityError || unsanitizedMessage.isEmpty());
    v8::Handle<v8::Value> exception = createDOMException(isolate, ec, sanitizedMessage, unsanitizedMessage, creationContext);
    if (exception.IsEmpty())
        return v8Undefined();

    return V8ThrowException::throwException(exception, isolate);
}
