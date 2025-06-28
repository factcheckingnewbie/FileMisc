#include "dsl_parser.h"
#include "dsl_defs.h"
#include <regex>
#include <stack>
#include <algorithm>

static int count_indent(const std::string& line) {
    int count = 0;
    for (char c : line) {
        if (c == ' ' || c == '\t') ++count;
        else break;
    }
    return count;
}

std::shared_ptr<DSLNode> DSLParser::parse(std::istream& in) {
    auto root = std::make_shared<DSLNode>();
    root->type = DSL::Root;
    root->name = "root";

    std::stack<std::shared_ptr<DSLNode>> node_stack;
    node_stack.push(root);

    std::string line;
    while (std::getline(in, line)) {
        std::string trimmed = line;
        trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);
        int indent = count_indent(line);

        int kind = DSL::node_kind_from_line(trimmed);
        int cat = DSL::kind_category(kind);

        auto node = std::make_shared<DSLNode>();
        node->type = kind;
        node->raw_line = line;

        // Skip categories by value, not by name
        if (DSL::is_skip_category(cat)) {
            node_stack.top()->children.push_back(node);
            continue;
        }
        if (cat == DSL::CatSeparator) {
            node->name = trimmed;
            node_stack.top()->children.push_back(node);
            continue;
        }
        if (cat == DSL::CatProperty) {
            auto pos = trimmed.find(':');
            node->name = trimmed.substr(0, pos);
            node->value = trimmed.substr(pos + 1);
            node_stack.top()->children.push_back(node);
            continue;
        }
        // Block or unknown
        node->name = trimmed;
        // Find parent by indent
        while (node_stack.size() > 1 && indent <= count_indent(node_stack.top()->raw_line))
            node_stack.pop();
        node_stack.top()->children.push_back(node);
        node_stack.push(node);
    }
    return root;
}

// Table-driven expand/retract logic for JSON
static ordered_json node_to_json_expand_retract(const std::shared_ptr<DSLNode>& node, const std::shared_ptr<DSLNode>& parent = nullptr) {
    ordered_json j;
    j["type"] = node->type;
    j["raw_line"] = node->raw_line;
    if (!node->name.empty()) j["name"] = node->name;
    if (!node->value.empty()) j["value"] = node->value;

    int kind = node->type;
    int parent_kind = parent ? parent->type : DSL::Unknown;

    int fold_dir = 0;
    if (parent) {
        fold_dir = DSL::fold_direction(parent_kind, kind);
    }

    bool has_foldable_child = false;
    for (const auto& child : node->children) {
        if (DSL::fold_direction(kind, child->type) != 0) {
            has_foldable_child = true;
            break;
        }
    }

    if (!node->children.empty()) {
        if (fold_dir == -1 && has_foldable_child) {
            // Retract: do not include children
        } else if (fold_dir == 1) {
            // Expand: include children
            ordered_json arr = ordered_json::array();
            for (const auto& child : node->children)
                arr.push_back(node_to_json_expand_retract(child, node));
            j["children"] = arr;
        } else if (fold_dir == 0) {
            // Normal: include children
            ordered_json arr = ordered_json::array();
            for (const auto& child : node->children)
                arr.push_back(node_to_json_expand_retract(child, node));
            j["children"] = arr;
        }
    }
    return j;
}

ordered_json DSLParser::to_json(const std::shared_ptr<DSLNode>& root) const {
    return node_to_json_expand_retract(root);
}