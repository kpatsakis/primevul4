static void test_hash_module()
{
  uint8_t blob[] = {0x61, 0x62, 0x63, 0x64, 0x65};

  assert_true_rule_blob(
      "import \"hash\" \
       rule test { \
        condition: \
          hash.md5(0, filesize) == \
            \"ab56b4d92b40713acc5af89985d4b786\" \
            and \
          hash.md5(1, filesize) == \
            \"e02cfbe5502b64aa5ae9f2d0d69eaa8d\" \
            and \
          hash.sha1(0, filesize) == \
            \"03de6c570bfe24bfc328ccd7ca46b76eadaf4334\" \
            and \
          hash.sha1(1, filesize) == \
            \"a302d65ae4d9e768a1538d53605f203fd8e2d6e2\" \
            and \
          hash.sha256(0, filesize) == \
            \"36bbe50ed96841d10443bcb670d6554f0a34b761be67ec9c4a8ad2c0c44ca42c\" \
            and \
          hash.sha256(1, filesize) == \
            \"aaaaf2863e043b9df604158ad5c16ff1adaf3fd7e9fcea5dcb322b6762b3b59a\" \
      }",
      blob);


  assert_true_rule_blob(
      "import \"hash\" \
       rule test { \
        condition: \
          hash.md5(0, filesize) == \
            \"ab56b4d92b40713acc5af89985d4b786\" \
            and \
          hash.md5(1, filesize) == \
            \"e02cfbe5502b64aa5ae9f2d0d69eaa8d\" \
            and \
          hash.md5(0, filesize) == \
            \"ab56b4d92b40713acc5af89985d4b786\" \
            and \
          hash.md5(1, filesize) == \
            \"e02cfbe5502b64aa5ae9f2d0d69eaa8d\" \
      }",
      blob);
}
