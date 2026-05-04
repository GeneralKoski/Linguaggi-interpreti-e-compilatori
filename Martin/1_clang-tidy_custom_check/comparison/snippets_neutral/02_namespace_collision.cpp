namespace mylib {
int printf(const char *);
}

extern "C" int printf(const char *, ...);

int caller() {
  mylib::printf("greeting from mylib");
  ::printf("hello from libc\n");
  return 0;
}
