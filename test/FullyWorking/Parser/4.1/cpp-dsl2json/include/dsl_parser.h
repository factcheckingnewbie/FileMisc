#pragma once
#include <string>
#include <vector>
#include <memory>
#include <istream>
#include <nlohmann/json.hpp>
#include "dsl_defs.h"

using ordered_json = nlohmann::ordered_json;

struct DSLNode {
    int type; // NodeKind as int
    std::string name;
    std::string value;
    std::string raw_line;
    std::vector<std::shared_ptr<DSLNode>> children;
};

class DSLParser {
public:
    DSLParser() = default;
    std::shared_ptr<DSLNode> parse(std::istream& in);
    ordered_json to_json(const std::shared_ptr<DSLNode>& root) const;
};