#include "json_parser.h"
#include "dsl_defs.h"
#include <string>
#include <ostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

// Helper: get DSL prefix for a node kind (int)
static std::string dsl_prefix_for_kind(int kind) {
    for (const auto& entry : DSL::kNodeKindTable) {
        if (entry.kind == kind)
            return std::string(entry.dsl_prefix);
    }
    return "";
}

// Helper: get indent string
static std::string indent_str(int indent) {
    return std::string(indent, ' ');
}

// Table-driven DSL writer
static void write_node(const ordered_json& node, std::ostream& out, int parent_kind = DSL::Unknown) {
    int kind = node.value("type", DSL::Unknown);
    int cat = DSL::kind_category(kind);
    int indent = node.value("indent", 0);

    // Skip categories by value, not by name
    if (DSL::is_skip_category(cat)) {
        // Write comment or empty line as is
        if (cat == DSL::CatComment) {
            std::string raw = node.value("raw_line", "");
            out << indent_str(indent) << raw << "\n";
        } else {
            out << "\n";
        }
        return;
    }

    std::string prefix = dsl_prefix_for_kind(kind);
    std::string name = node.value("name", "");
    std::string value = node.value("value", "");
    std::string raw = node.value("raw_line", "");

    // Property
    if (cat == DSL::CatProperty) {
        out << indent_str(indent) << name << ":" << value << "\n";
        return;
    }

    // Separator
    if (cat == DSL::CatSeparator) {
        out << indent_str(indent) << prefix << "\n";
        return;
    }

    // Block, Root, etc.
    if (!prefix.empty()) {
        out << indent_str(indent) << prefix;
        if (!name.empty()) {
            if (name.find(prefix) == 0)
                out << name.substr(prefix.size());
            else
                out << " " << name;
        }
        if (!value.empty()) {
            out << " " << value;
        }
        out << "\n";
    } else if (!raw.empty()) {
        out << indent_str(indent) << raw << "\n";
    }

    // Write children recursively
    if (node.contains("children") && node["children"].is_array()) {
        for (const auto& child : node["children"]) {
            write_node(child, out, kind);
        }
    }
}

void JSON2DSLWriter::write_dsl(const ordered_json& root, std::ostream& out) const {
    write_node(root, out);
}