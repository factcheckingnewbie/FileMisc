#pragma once
#include <string_view>
#include <array>
#include <set>
#include <string>

namespace DSL {

// All values are just integers, not symbolic names
enum NodeKind : int {
    FoldableMenu = 0,
    Submenu,
    Item,
    Separator,
    Comment,
    EmptyLine,
    Property,
    Block,
    Root,
    Unknown
};

enum NodeCategory : int {
    CatComment = 0,
    CatEmptyLine,
    CatSeparator,
    CatProperty,
    CatBlock,
    CatRoot,
    CatUnknown
};

struct NodeKindEntry {
    int kind; // NodeKind as int
    std::string_view dsl_prefix;
};

struct NodeKindCategoryEntry {
    int kind;      // NodeKind as int
    int category;  // NodeCategory as int
};

struct FoldRule {
    int parent; // NodeKind as int
    int child;  // NodeKind as int
    int direction; // -1 = retract, 0 = neutral, 1 = expand
};

inline constexpr std::array<NodeKindEntry, 5> kNodeKindTable = {{
    {FoldableMenu, "FoldableMenu"},
    {Submenu,      "Submenu"},
    {Item,         "Item"},
    {Separator,    "Separator"},
    {Comment,      ""} // Comment handled by marker
}};

inline constexpr std::array<NodeKindCategoryEntry, 7> kKindCategoryTable = {{
    {Comment,   CatComment},
    {EmptyLine, CatEmptyLine},
    {Separator, CatSeparator},
    {Property,  CatProperty},
    {Block,     CatBlock},
    {Root,      CatRoot},
    {Unknown,   CatUnknown}
}};

inline constexpr std::array<std::string_view, 2> kCommentMarkers = {
    "#",
    "//"
};

inline constexpr std::array<FoldRule, 2> kFoldTable = {{
    {FoldableMenu, Submenu, 1},   // Expand
    {Submenu, FoldableMenu, -1}   // Retract
}};

// Set of categories to skip (as integer values)
inline const std::set<int> kSkipCategories = {CatComment, CatEmptyLine};

inline int node_kind_from_line(const std::string& line) {
    // Comment
    for (auto& marker : kCommentMarkers) {
        if (line.find(marker) == 0) return Comment;
    }
    // EmptyLine
    if (line.find_first_not_of(" \t\r\n") == std::string::npos)
        return EmptyLine;
    // Table-driven lookup
    for (const auto& entry : kNodeKindTable) {
        if (!entry.dsl_prefix.empty() && line.find(entry.dsl_prefix) == 0)
            return entry.kind;
    }
    // Property: contains ':'
    if (line.find(':') != std::string::npos)
        return Property;
    return Unknown;
}

inline int kind_category(int kind) {
    for (const auto& entry : kKindCategoryTable) {
        if (entry.kind == kind)
            return entry.category;
    }
    return CatUnknown;
}

inline bool is_skip_category(int cat) {
    return kSkipCategories.count(cat) > 0;
}

inline int fold_direction(int parent, int child) {
    for (const auto& rule : kFoldTable) {
        if (rule.parent == parent && rule.child == child)
            return rule.direction;
    }
    return 0;
}

} // namespace DSL