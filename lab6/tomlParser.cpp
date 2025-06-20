#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <regex>
#include <sstream>
#include <filesystem>

class TOMLParser {
private:
    std::map<std::string, std::map<std::string, std::string>> sections;
    std::string current_section;

    void generate_state_diagram(const std::string& filename) const {
        std::ofstream dot_file(filename);
        if (!dot_file.is_open()) {
            throw std::runtime_error("Cannot create DOT file: " + filename);
        }

        // Write DOT file with complete styling
        dot_file << "digraph TOMLParser {\n";
        dot_file << "    // Graph settings\n";
        dot_file << "    rankdir=LR;\n";
        dot_file << "    size=\"8,5\";\n";
        dot_file << "    node [shape=circle, style=filled, fillcolor=lightblue, fontname=\"Arial\"];\n";
        dot_file << "    edge [fontname=\"Arial\"];\n\n";

        // Define states with styling
        dot_file << "    // States\n";
        dot_file << "    Start [shape=point, fillcolor=black];\n";
        dot_file << "    Section [label=\"Section\", fillcolor=\"#FFB6C1\"];\n";
        dot_file << "    Key [label=\"Key\", fillcolor=\"#98FB98\"];\n";
        dot_file << "    Value [label=\"Value\", fillcolor=\"#87CEEB\"];\n";
        dot_file << "    Comment [label=\"Comment\", fillcolor=\"#DDA0DD\"];\n";
        dot_file << "    End [shape=doublecircle, fillcolor=\"#F0E68C\"];\n\n";

        // Define transitions with styling
        dot_file << "    // Transitions\n";
        dot_file << "    Start -> Section [label=\"[\", color=blue];\n";
        dot_file << "    Start -> Key [label=\"char\", color=green];\n";
        dot_file << "    Start -> Comment [label=\"#\", color=purple];\n\n";
        
        dot_file << "    Section -> Section [label=\"char\", color=red];\n";
        dot_file << "    Section -> Key [label=\"\\n\", color=blue];\n\n";
        
        dot_file << "    Key -> Key [label=\"char\", color=green];\n";
        dot_file << "    Key -> Value [label=\"=\", color=blue];\n\n";
        
        dot_file << "    Value -> Value [label=\"char\", color=blue];\n";
        dot_file << "    Value -> End [label=\"\\n\", color=red];\n\n";
        
        dot_file << "    Comment -> Comment [label=\"char\", color=purple];\n";
        dot_file << "    Comment -> End [label=\"\\n\", color=red];\n\n";

        // Add legend
        dot_file << "    // Legend\n";
        dot_file << "    subgraph cluster_legend {\n";
        dot_file << "        label=\"Legend\";\n";
        dot_file << "        style=filled;\n";
        dot_file << "        color=lightgrey;\n";
        dot_file << "        node [shape=box, style=filled, fillcolor=white];\n";
        dot_file << "        edge [style=invis];\n";
        dot_file << "        legend1 [label=\"Initial State\"];\n";
        dot_file << "        legend2 [label=\"Final State\"];\n";
        dot_file << "        legend3 [label=\"Section State\"];\n";
        dot_file << "        legend4 [label=\"Key State\"];\n";
        dot_file << "        legend5 [label=\"Value State\"];\n";
        dot_file << "        legend6 [label=\"Comment State\"];\n";
        dot_file << "    }\n";

        dot_file << "}\n";
    }

public:
    TOMLParser() : current_section("") {}

    void parse(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }

            // Remove comments from the end of the line
            size_t comment_pos = line.find('#');
            if (comment_pos != std::string::npos) {
                line = line.substr(0, comment_pos);
            }

            // Trim whitespace
            line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");

            // Parse section header
            if (line[0] == '[' && line[line.length() - 1] == ']') {
                current_section = line.substr(1, line.length() - 2);
                continue;
            }

            // Parse key-value pair
            size_t equal_pos = line.find('=');
            if (equal_pos != std::string::npos) {
                std::string key = line.substr(0, equal_pos);
                std::string value = line.substr(equal_pos + 1);

                // Trim whitespace from key and value
                key = std::regex_replace(key, std::regex("^\\s+|\\s+$"), "");
                value = std::regex_replace(value, std::regex("^\\s+|\\s+$"), "");

                // Remove quotes if present
                if (value.length() >= 2 && value[0] == '"' && value[value.length() - 1] == '"') {
                    value = value.substr(1, value.length() - 2);
                }

                sections[current_section][key] = value;
            }
        }
    }

    void print() const {
        for (const auto& section : sections) {
            if (!section.first.empty()) {
                std::cout << "[" << section.first << "]" << std::endl;
            }
            for (const auto& pair : section.second) {
                std::cout << pair.first << " = " << pair.second << std::endl;
            }
            std::cout << std::endl;
        }
    }

    void save_to_csv(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create file: " + filename);
        }

        file << "section,key,value\n";
        for (const auto& section : sections) {
            for (const auto& pair : section.second) {
                file << section.first << ","
                     << pair.first << ","
                     << pair.second << "\n";
            }
        }
    }

    void generate_diagram(const std::string& filename) const {
        generate_state_diagram(filename);
    }
};

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input.toml> <output.csv> <diagram.png>" << std::endl;
        return 1;
    }

    try {
        TOMLParser parser;
        parser.parse(argv[1]);
        parser.save_to_csv(argv[2]);
        parser.generate_diagram(argv[3]);
        std::cout << "Parsing completed successfully!" << std::endl;
        std::cout << "State diagram has been generated!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} 