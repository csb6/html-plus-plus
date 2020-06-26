#include <iostream>
#include "html++.h"

int main()
{
    constexpr char panda_url[] = "src='https://upload.wikimedia.org/wikipedia/commons/f/fa/Panda_bear_at_memphis_zoo.JPG'";

    html<
        head<
            title<"HTML++ = C++ Templates + HTML">
        >,
        body<
            h1<"This was a mistake">,
            h2<"Here are some pictures:">,
            img<panda_url, "alt='A panda bear'", "width=300">,
            br,
            h2<"Here is some text">,
            div_<
                p<"Hello, World!">,
                p<"Yes, this document structure has been typechecked :)">
            >,
            a<"href='https://en.cppreference.com/'", "This website is helpful">
        >
    > page;

    std::cout << page.content;

    return 0;
}
