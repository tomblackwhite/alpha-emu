#include <iostream>
int main()
{
  int i=6;
  auto my=[&i]{std::cout << i;};
  my();
  i=9;
  my();
    return 0;
}
