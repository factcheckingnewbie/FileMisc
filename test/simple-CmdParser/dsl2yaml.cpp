#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Function to escape shell commands in the yaml file
string escape_shell(string command) {
    string escaped = "";
    for (int i = 0; i < command.length(); i++) {
        if (command[i] == '"') {
            escaped += "\\\"";
        } else if (command[i] == '`') {
            escaped += "\\`";
        } else {
            escaped += command[i];
        }
    }
    return escaped;
}

int main() {
    // Open the dls-file and read its contents
    ifstream infile("dls-file.txt");
    string line;
    vector<string> lines;
    while (getline(infile, line)) {
        lines.push_back(line);
    }

    // Create a new yaml file and write its contents
    ofstream outfile("yaml-file.txt");
    for (int i = 0; i < lines.size(); i++) {
        string line = lines[i];
        if (line == "Commands") {
            // Add a new section to the yaml file
            outfile << "---" << endl;
            outfile << "Command: list_files" << endl;
            outfile << "Description: \"List files in a directory\"" << endl;
            outfile << "Args:" << endl;
            outfile << "- directory" << endl;
            outfile << "Out:" << endl;
            outfile << "- filelist" << endl;
            outfile << "Shell:" << endl;
            outfile << escape_shell("ls \"{directory}\"") << endl;
        } else if (line == "Command: find_pattern") {
            // Add a new section to the yaml file
            outfile << "---" << endl;
            outfile << "Command: find_pattern" << endl;
            outfile << "Description: \"Find a pattern in a file list\"" << endl;
            outfile << "Args:" << endl;
            outfile << "- pattern" << endl;
            outfile << "- in:filelist" << endl;
            outfile << "Out:" << endl;
            outfile << "- matches" << endl;
            outfile << "Shell:" << endl;
            outfile << escape_shell("echo \"{in}\" | grep \"{pattern}\"") << endl;
        } else if (line == "Command: count_matches") {
            // Add a new section to the yaml file
            outfile << "---" << endl;
            outfile << "Command: count_matches" << endl;
            outfile << "Description: \"Count number of matches\"" << endl;
            outfile << "Args:" << endl;
            outfile << "- in:matches" << endl;
            outfile << "Out:" << endl;
            outfile << "- count" << endl;
            outfile << "Shell:" << endl;
            outfile << escape_shell("echo \"{in}\" | wc -l") << endl;
        } else if (line == "Pipeline: Count files matching foo") {
            // Add a new section to the yaml file
            outfile << "---" << endl;
            outfile << "Pipeline: Count files matching foo" << endl;
            outfile << "- Step:" << endl;
            outfile << "- list_files directory:\"/tmp\"" << endl;
            outfile << "- find_pattern pattern:\"foo\" in:@list_files" << endl;
            outfile << "- count_matches in:@find_pattern" << endl;
        } else {
            // Add a new line to the yaml file
            outfile << line << endl;
        }
    }

    return 0;
}

