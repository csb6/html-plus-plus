#include <iostream>
#include "html++.h"

int main()
{
  html<
    head<
      title<"Help Me.">
    >,
    body<
      h1<"The horror!">,
      p<"Someone has probably done this before, but I can see why it didn't catch on.">,
      a<"href=https://github.com/csb6/html++", "For science">
    >
  > page;

  std::cout << page.content;

  return 0;
}
