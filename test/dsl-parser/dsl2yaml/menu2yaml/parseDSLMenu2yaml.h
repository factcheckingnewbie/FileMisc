#pragma once
#include <QString>

// Parse menu-section DSL to YAML, write output to yamlPath.
// Returns true if parsing and writing succeed, false otherwise.
bool parseDSLMenuToYaml(const QString &filePath, const QString &yamlPath);
