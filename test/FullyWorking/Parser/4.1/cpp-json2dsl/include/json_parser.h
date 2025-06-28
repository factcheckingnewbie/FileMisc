#pragma once
#include <string>
#include <ostream>
#include <nlohmann/json.hpp>
#include "dsl_defs.h"

using ordered_json = nlohmann::ordered_json;

class JSON2DSLWriter {
public:
    JSON2DSLWriter() = default;
    void write_dsl(const ordered_json& root, std::ostream& out, int indent_size = 2) const;
};