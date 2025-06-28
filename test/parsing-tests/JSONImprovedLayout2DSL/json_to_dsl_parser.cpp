#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class JsonToDslParser {
private:
    struct Context {
        int indentLevel;
        std::string parentType;
        bool inRadioGroup;
        
        Context(int level = 0, const std::string& parent = "", bool radio = false) 
            : indentLevel(level), parentType(parent), inRadioGroup(radio) {}
    };
    
    std::stack<Context> contextStack;
    std::ostringstream output;
    
    std::string getIndent(int level) const {
        return std::string(level * 4, ' ');
    }
    
    bool isContainerType(const std::string& type) const {
        static const std::vector<std::string> containers = {
            "Window", "Panel", "FoldableMenu", "Submenu", "Tab", 
            "Dialog", "GroupBox", "RadioGroup", "TabView"
        };
        return std::find(containers.begin(), containers.end(), type) != containers.end();
    }
    
    bool shouldIncrementIndent(const std::string& type) const {
        static const std::vector<std::string> indentTypes = {
            "FoldableMenu", "Submenu", "Tab", "GroupBox", "RadioGroup", "TabView"
        };
        return std::find(indentTypes.begin(), indentTypes.end(), type) != indentTypes.end();
    }
    
    std::string formatAttributeValue(const json& value) const {
        if (value.is_string()) {
            std::string str = value.get<std::string>();
            // Check if value contains spaces or special characters that need quotes
            if (str.find(' ') != std::string::npos || 
                str.find(',') != std::string::npos ||
                str.find(':') != std::string::npos) {
                return "\"" + str + "\"";
            }
            return str;
        } else if (value.is_boolean()) {
            return value.get<bool>() ? "true" : "false";
        } else if (value.is_number()) {
            if (value.is_number_integer()) {
                return std::to_string(value.get<int>());
            } else {
                return std::to_string(value.get<double>());
            }
        }
        return value.dump();
    }
    
    std::string formatAttributes(const json& attributes) const {
        std::ostringstream attrs;
        std::vector<std::string> attrPairs;
        
        for (auto& [key, value] : attributes.items()) {
            if (value.is_array()) {
                // Handle array attributes like sizes
                std::string arrayStr = key + ":";
                bool first = true;
                for (auto& elem : value) {
                    if (!first) arrayStr += ",";
                    arrayStr += formatAttributeValue(elem);
                    first = false;
                }
                attrPairs.push_back(arrayStr);
            } else {
                attrPairs.push_back(key + ":" + formatAttributeValue(value));
            }
        }
        
        // Special ordering for position attributes
        std::vector<std::string> orderedAttrs;
        std::vector<std::string> otherAttrs;
        
        for (const auto& attr : attrPairs) {
            if (attr.find("position:") == 0 || attr.find("width:") == 0 || 
                attr.find("height:") == 0 || attr.find("id:") == 0) {
                orderedAttrs.push_back(attr);
            } else {
                otherAttrs.push_back(attr);
            }
        }
        
        // Combine ordered and other attributes
        orderedAttrs.insert(orderedAttrs.end(), otherAttrs.begin(), otherAttrs.end());
        
        for (size_t i = 0; i < orderedAttrs.size(); ++i) {
            if (i > 0) attrs << " ";
            attrs << orderedAttrs[i];
        }
        
        return attrs.str();
    }
    
    void processElement(const json& element, int indentLevel) {
        std::string type = element.value("type", "");
        std::string name = element.value("name", "");
        json attributes = element.value("attributes", json::object());
        
        // Skip root type
        if (type == "root") return;
        
        // Handle special cases
        if (type == "Separator") {
            output << getIndent(indentLevel) << "Separator\n";
            return;
        }
        
        // Handle comments
        if (type == "comment") {
            output << "\n# " << name << "\n\n";
            return;
        }
        
        // Build element string
        std::ostringstream elementStr;
        elementStr << getIndent(indentLevel) << type;
        
        if (!name.empty()) {
            elementStr << " \"" << name << "\"";
        }
        
        std::string attrs = formatAttributes(attributes);
        if (!attrs.empty()) {
            elementStr << " " << attrs;
        }
        
        output << elementStr.str() << "\n";
        
        // Process children if this is a container
        if (element.contains("children") && element["children"].is_array()) {
            int childIndent = shouldIncrementIndent(type) ? indentLevel + 1 : indentLevel;
            
            for (const auto& child : element["children"]) {
                processElement(child, childIndent);
            }
        }
    }
    
    void processJsonStructure(const json& root) {
        if (root.contains("children") && root["children"].is_array()) {
            std::vector<json> dialogElements;
            std::vector<json> otherElements;
            
            // Separate dialogs from other elements
            for (const auto& child : root["children"]) {
                std::string type = child.value("type", "");
                if (type == "Dialog") {
                    dialogElements.push_back(child);
                } else {
                    otherElements.push_back(child);
                }
            }
            
            // Process non-dialog elements first
            int currentIndent = 0;
            for (size_t i = 0; i < otherElements.size(); ++i) {
                const auto& element = otherElements[i];
                std::string type = element.value("type", "");
                
                // Add comments before certain sections
                if (i > 0 && type == "Dialog" && otherElements[i-1].value("type", "") != "Dialog") {
                    output << "\n# Dialog definitions for commands\n\n";
                } else if (type == "Panel" && element.value("attributes", json::object()).contains("statusbar")) {
                    output << "\n# Status bar at bottom\n";
                }
                
                processElement(element, currentIndent);
            }
            
            // Process dialog elements
            if (!dialogElements.empty()) {
                output << "\n# Dialog definitions for commands\n\n";
                for (const auto& dialog : dialogElements) {
                    processElement(dialog, 0);
                    output << "\n";
                }
            }
        }
    }
    
public:
    std::string parse(const json& jsonData) {
        output.str("");
        output.clear();
        
        processJsonStructure(jsonData);
        
        return output.str();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.json> <output.dsl>\n";
        return 1;
    }
    
    try {
        // Read JSON file
        std::ifstream inputFile(argv[1]);
        if (!inputFile.is_open()) {
            std::cerr << "Error: Could not open input file: " << argv[1] << "\n";
            return 1;
        }
        
        json jsonData;
        inputFile >> jsonData;
        inputFile.close();
        
        // Parse JSON to DSL
        JsonToDslParser parser;
        std::string dslOutput = parser.parse(jsonData);
        
        // Write DSL file
        std::ofstream outputFile(argv[2]);
        if (!outputFile.is_open()) {
            std::cerr << "Error: Could not open output file: " << argv[2] << "\n";
            return 1;
        }
        
        outputFile << dslOutput;
        outputFile.close();
        
        std::cout << "Successfully converted " << argv[1] << " to " << argv[2] << "\n";
        
    } catch (const json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
