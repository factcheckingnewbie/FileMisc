#pragma once
#include <QString>

// Parse command-section YAML to DSL, write output to dslPath.
// Returns true if parsing and writing succeed, false otherwise.
bool parseYAMLCmd2DSL(const QString &yamlPath, const QString &dslPath);
