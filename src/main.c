extern void main_test();

int main() {
#ifdef RUN_TEST
  main_test();
  return 0;
#endif
}
