  NetworkLibraryStubImpl()
      : ip_address_("1.1.1.1"),
        ethernet_(new EthernetNetwork()),
        wifi_(NULL),
        cellular_(NULL) {
  }
