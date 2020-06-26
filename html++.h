#pragma once
#include <iostream>
#include <string>
#include <type_traits>

#define STATIC_ASSERT_CHILDREN_ARE_VALID(BASE_TAG, TAG_NAME) \
    static_assert(((std::is_base_of_v<BASE_TAG, Children> || std::is_same_v<void, Children>) && ...), \
                  "<" #TAG_NAME "> must contain type 'void' or an element that is a subclass of " #BASE_TAG  " for all template parameters");

// Not yet implemented in GCC 9 std lib; will be in C++20, but I had
// to add it here for now
template< class InputIt, class Size, class OutputIt>
constexpr OutputIt copy_n(InputIt first, Size count, OutputIt result)
{
    if (count > 0) {
        *result++ = *first;
        for (Size i = 1; i < count; ++i) {
            *result++ = *++first;
        }
    }
    return result;
}

constexpr void add_indent(std::string &page_content, unsigned indent)
{
    for(unsigned i = 0; i < indent; ++i) {
        page_content += "    ";
    }
}

template<size_t N>
struct StringLiteral {
    constexpr StringLiteral(const char (&str)[N])
    {
        copy_n(str, N, value);
    }

    char value[N]{};
    constexpr bool empty() const { return N == 0; }
    constexpr const char* c_str() const { return &value[0]; }
};


struct add_tag {
    add_tag(std::string &page_content_, const char *tag_, unsigned indent_ = 0)
        : add_tag(page_content_, tag_, indent_, false)
    {}

    template<typename ...Attributes>
    add_tag(std::string &page_content_, const char *tag_, unsigned indent_,
            bool is_self_closing_, Attributes... attributes)
        : page_content(page_content_), tag(tag_), indent(indent_),
          is_self_closing(is_self_closing_)
    {
        static_assert((std::is_convertible_v<Attributes, const char*> && ...),
                      "HTML attributes must be string literals");
        add_indent(page_content, indent);
        page_content += '<';
        page_content += tag;
        auto add_attributes = [this](const auto each) {
                                  if(each != nullptr && each[0] != '\0') {
                                      page_content += ' ';
                                      page_content += each;
                                  }
                              };

        (add_attributes(attributes), ...);
        page_content += ">\n";
    }

    ~add_tag()
    {
        if (!is_self_closing) {
            add_indent(page_content, indent);
            page_content += "</";
            page_content += tag;
            page_content += ">\n";
        }
    }
private:
    std::string &page_content;
    const char *tag;
    unsigned indent;
    bool is_self_closing;
};


// Identifies the <head> HTML tag
struct head_tag {};
// Identifies an HTML tag that can legally be within <head>
struct head_element_tag {};

template<typename ...Children>
struct head : head_tag {
    STATIC_ASSERT_CHILDREN_ARE_VALID(head_element_tag, head)
    explicit head(std::string &page_content)
    {
        add_tag scope_guard(page_content, "head", 1);
        (Children(page_content, 2), ...);
    }
};


template<StringLiteral Content>
struct title : head_element_tag {
    title(std::string &page_content, unsigned indent)
    {
        add_tag scope_guard(page_content, "title", indent);
        add_indent(page_content, indent + 1);
        page_content += Content.c_str();
        page_content += '\n';
    }
};


// Identifies the <body> HTML tag
struct body_tag {};
// Identifies an HTML tag that can legally be within <body>
struct body_element_tag {};

template<typename ...Children>
struct body : body_tag {
    STATIC_ASSERT_CHILDREN_ARE_VALID(body_element_tag, body)
    explicit body(std::string &page_content)
    {
        add_tag scope_guard(page_content, "body", 1);
        (Children(page_content, 2), ...);
    }
};


template<typename head_ = void, typename body_ = void>
struct html {
    static_assert(std::is_base_of_v<head_tag, head_> || std::is_same_v<void, head_>,
                  "<html> must contain type 'void' or type 'head' as its first template parameter");
    static_assert(std::is_base_of_v<body_tag, body_> || std::is_same_v<void, body_>,
                  "<html> must contain type 'void' or type 'body' as its second template parameter");
    html()
    {
        add_tag scope_guard(content, "html");
        if constexpr(!std::is_same_v<void, head_>) {
                head_{content};
        }

        if constexpr(!std::is_same_v<void, body_>) {
                body_{content};
        }
    }

    std::string content;
};

// Form 5: body_element with attributes but no content
template<StringLiteral Src = "", StringLiteral Alt = "", StringLiteral Width = "">
struct img : body_element_tag {
    img(std::string &page_content, unsigned indent)
    {
        add_tag scope_guard(page_content, "img", indent, true, Src.value, Alt.value, Width.value);
    }
};

// Form 4: body_element with no attributes and no content
struct br : body_element_tag {
    br(std::string &page_content, unsigned indent)
    {
        add_tag scope_guard(page_content, "br", indent, true);
    }
};

// Form 3: body_element with attributes and content
template<StringLiteral Href = "", StringLiteral Content = "", typename ...Children>
struct a : body_element_tag {
    a(std::string &page_content, unsigned indent)
    {
        add_tag scope_guard(page_content, "a", indent, false, Href.value);
        if constexpr (!Content.empty()) {
                add_indent(page_content, indent + 1);
                page_content += Content.c_str();
                page_content += '\n';
        }
        (Children(page_content, indent + 1), ...);
    }
};


// Form 2: body_element with no content but children
template<typename ...Children>
struct div_ : body_element_tag {
    STATIC_ASSERT_CHILDREN_ARE_VALID(body_element_tag, div)
    div_(std::string &page_content, unsigned indent)
    {
        add_tag scope_guard(page_content, "div", indent);
        (Children(page_content, indent + 1), ...);
    }
};

// Form 1: body_element with content and children
template<StringLiteral Content = "", typename ...Children>
struct p : body_element_tag {
    STATIC_ASSERT_CHILDREN_ARE_VALID(body_element_tag, p)
    p(std::string &page_content, unsigned indent)
    {
        add_tag scope_guard(page_content, "p", indent);
        if constexpr (!Content.empty()) {
                add_indent(page_content, indent + 1);
                page_content += Content.c_str();
                page_content += '\n';
        }
        (Children(page_content, indent + 1), ...);
    }
};

template<StringLiteral Content = "", typename ...Children>
struct h1 : body_element_tag {
    STATIC_ASSERT_CHILDREN_ARE_VALID(body_element_tag, h1)
    h1(std::string &page_content, unsigned indent)
    {
        add_tag scope_guard(page_content, "h1", indent);
        if constexpr (!Content.empty()) {
                add_indent(page_content, indent + 1);
                page_content += Content.c_str();
                page_content += '\n';
        }
        (Children(page_content, indent + 1), ...);
    }
};

template<StringLiteral Content = "", typename ...Children>
struct h2 : body_element_tag {
    STATIC_ASSERT_CHILDREN_ARE_VALID(body_element_tag, h2)
    h2(std::string &page_content, unsigned indent)
    {
        add_tag scope_guard(page_content, "h2", indent);
        if constexpr (!Content.empty()) {
                add_indent(page_content, indent + 1);
                page_content += Content.c_str();
                page_content += '\n';
        }
        (Children(page_content, indent + 1), ...);
    }
};
