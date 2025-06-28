#pragma once
#include <QString>

// Parse command-section DSL to YAML, write output to yamlPath.
// Returns true if parsing and writing succeed, false otherwise.
bool parseDSLCommandsToYaml(const QString &filePath, const QString &yamlPath);
// Parse menu-section DSL to YAML, write output to yamlPath.
// Returns true if parsing and writing succeed, false otherwise.
bool parseDSLMenuToYaml(const QString &filePath, const QString &yamlPath);
