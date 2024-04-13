  static void NetworkManagerStatusChangedHandler(void* object,
                                                 const char* path,
                                                 const char* key,
                                                 const Value* value) {
    NetworkLibraryImpl* networklib = static_cast<NetworkLibraryImpl*>(object);
    DCHECK(networklib);
    networklib->UpdateNetworkManagerStatus();
  }
