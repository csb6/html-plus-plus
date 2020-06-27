# HTML++

Write HTML using C++ templates. The HTML document is represented as a single, deeply-
nested type which is type-checked by the compiler using certain rules about how
HTML elements are allowed to be nested (e.g. nothing can be nested inside a `<br>` tag).

If compilation succeeds, you will have a program that prints an HTML document
to the standard output when run, with readable indentation.

## Example

Say you want to write the following HTML page:

```html
<html>
  <head>
    <title>Help Me.</title>
  </head>
  <body>
    <h1>The horror!</h1>
    <p>
      Someone has probably done this before, but I can see why they haven't
      told anyone about it.
    </p>
    <a href="https://github.com/csb6/html++">For science</a>
  </body>
</html>
```

Here is a C++ program that can be used to generate that page:

```cpp
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
      a<"href=https://github.com/csb6/html-plus-plus", "For science">
    >
  > page;


  std::cout << page.content;

  return 0;
}
```

## Installation

This library require C++20. It works with GCC 9.2.0 with `-std=c++2a` enabled.
It doesn't work with Apple Clang 11.0, but it might work on other compilers. If
C++20 is fully implemented on a given compiler, it should be able to compile.

Simply `#include html++.h`. It is the only file you need.

## Questions

**Why**

I was writing some HTML, and I realized that the structure and syntax of
HTML tags was quite a bit like the structure/syntax of C++ templates. Both
enable you to nest identifiers into a tree structure.

Since variadic templates were added to C++, a template can hold any
number of other types in a parameter pack, enabling parent nodes to hold any
number of child nodes. This is necessary in order for HTML elements to be
properly represented by C++ types.

Since C++20, it is now possible to use string literals as non-type template
parameters (e.g. `h1<"This is a title">`), making C++ templates capable of
imitating the appearance of HTML tags even more closely.

I thought I'd see how horrible it would be, and, as expected, it is pretty
ridiculous.

**How**

The entire library is basically a fancy way of concatenating strings.
Each tag is defined as its own template struct
(e.g. `template<...> struct h1 { ... };`). Each tag takes 0 or more type/
non-type template parameters. Template parameters can be HTML attributes
(e.g. `"img<src='pic.png'", "alt='A picture'">`) or an arbitrarily long list of
other element types, which can themselves hold other types as child nodes
(e.g. `html<head<title<...>>, body<...>>`).

Type safety can be achieved by defining only template parameters that make sense for
a tag (e.g. `<img>` is a self-closing tag, so it would not make sense for it to accept a
template parameter pack of child nodes). Using inheritance and
`static_assert`, along with "phantom" types (e.g. `img` inherits from an empty struct
named `body_element_tag`), ensures that the tags make semantic sense as children
of a given node. In this way, HTML can be given a degree of type-checking.

The output text is assembled by pre-order traversing the tree of types, calling each type's
constructor recursively. Each element adds its opening tag (e.g. "<html>\n") to a string
that is then passed by reference to each child element recursively. Once all children have
added their opening tags, each node adds its closing tag (e.g. "</html>") and returns from its
constructor. The string is stored in a member of the top-level node (`html`) and can be
printed and/or used like a normal string at runtime. The string is assembled at runtime;
however, the structure of the document is defined at compile-time.

**Should I use it**

No, but I think the type-checking aspect could be useful. I haven't added all HTML tags,
but in theory this library could be extended in such a way that you could write HTML
with somewhat strong typing, which might be useful for ensuring HTML standards conformance.

Hope this project is interesting (and concerning) to you!