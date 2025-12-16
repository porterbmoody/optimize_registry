#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <map>

// HL7 Parser - Parses HL7 v2.x messages
class HL7Parser {
private:
    char field_separator = '|';
    char component_separator = '^';
    char repetition_separator = '~';
    char escape_char = '\\';
    char subcomponent_separator = '&';
    
    std::vector<std::string> segments;
    std::map<std::string, std::vector<std::string>> parsed_segments;

public:
    // Parse complete HL7 message
    void parse(const std::string& message) {
        segments.clear();
        parsed_segments.clear();
        
        // Split by newlines to get segments
        std::stringstream ss(message);
        std::string segment;
        
        while (std::getline(ss, segment, '\n')) {
            if (!segment.empty() && segment != "\r") {
                segments.push_back(segment);
                parse_segment(segment);
            }
        }
    }
    
    // Parse individual segment
    void parse_segment(const std::string& segment) {
        std::vector<std::string> fields;
        std::stringstream ss(segment);
        std::string field;
        
        // Get segment type (first 3 chars)
        std::string segment_type = segment.substr(0, 3);
        
        // Special handling for MSH segment (has encoding characters)
        if (segment_type == "MSH") {
            fields.push_back("MSH");
            fields.push_back("|"); // Field separator
            
            // Parse rest of MSH
            std::string rest = segment.substr(4);
            std::stringstream rest_ss(rest);
            while (std::getline(rest_ss, field, field_separator)) {
                fields.push_back(field);
            }
        } else {
            // Parse normal segment
            while (std::getline(ss, field, field_separator)) {
                fields.push_back(field);
            }
        }
        
        parsed_segments[segment_type] = fields;
    }
    
    // Get field from segment
    std::string get_field(const std::string& segment_type, int field_index) {
        if (parsed_segments.find(segment_type) != parsed_segments.end()) {
            auto fields = parsed_segments[segment_type];
            if (field_index < fields.size()) {
                return fields[field_index];
            }
        }
        return "";
    }
    
    // Parse components within a field (split by ^)
    std::vector<std::string> parse_components(const std::string& field) {
        std::vector<std::string> components;
        std::stringstream ss(field);
        std::string component;
        
        while (std::getline(ss, component, component_separator)) {
            components.push_back(component);
        }
        
        return components;
    }
    
    // Extract patient name from PID segment
    std::string get_patient_name() {
        std::string pid_5 = get_field("PID", 5);
        auto components = parse_components(pid_5);
        
        if (components.size() >= 2) {
            return components[1] + " " + components[0]; // FirstName LastName
        }
        return "";
    }
    
    // Extract patient ID
    std::string get_patient_id() {
        std::string pid_3 = get_field("PID", 3);
        auto components = parse_components(pid_3);
        
        if (!components.empty()) {
            return components[0];
        }
        return "";
    }
    
    // Extract message type
    std::string get_message_type() {
        std::string msh_9 = get_field("MSH", 8);
        auto components = parse_components(msh_9);
        
        if (!components.empty()) {
            return components[0];
        }
        return "";
    }
    
    // Validate message structure
    bool validate() {
        // Must have MSH segment
        if (parsed_segments.find("MSH") == parsed_segments.end()) {
            std::cout << "ERROR: Missing MSH segment" << std::endl;
            return false;
        }
        
        // MSH must have required fields
        auto msh = parsed_segments["MSH"];
        if (msh.size() < 9) {
            std::cout << "ERROR: MSH segment incomplete" << std::endl;
            return false;
        }
        
        std::cout << "Message validation: PASSED" << std::endl;
        return true;
    }
    
    // Print parsed message
    void print() {
        std::cout << "\n=== PARSED HL7 MESSAGE ===" << std::endl;
        std::cout << "Message Type: " << get_message_type() << std::endl;
        std::cout << "Patient ID: " << get_patient_id() << std::endl;
        std::cout << "Patient Name: " << get_patient_name() << std::endl;
        
        std::cout << "\nAll Segments:" << std::endl;
        for (const auto& seg : parsed_segments) {
            std::cout << seg.first << " (" << seg.second.size() << " fields)" << std::endl;
        }
    }
    
    // Export to JSON-like format
    void export_json(const std::string& filename) {
        std::ofstream file(filename);
        
        file << "{\n";
        file << "  \"message_type\": \"" << get_message_type() << "\",\n";
        file << "  \"patient_id\": \"" << get_patient_id() << "\",\n";
        file << "  \"patient_name\": \"" << get_patient_name() << "\",\n";
        file << "  \"segments\": {\n";
        
        int seg_count = 0;
        for (const auto& seg : parsed_segments) {
            if (seg_count > 0) file << ",\n";
            file << "    \"" << seg.first << "\": [";
            
            for (size_t i = 0; i < seg.second.size(); i++) {
                if (i > 0) file << ", ";
                file << "\"" << seg.second[i] << "\"";
            }
            file << "]";
            seg_count++;
        }
        
        file << "\n  }\n";
        file << "}\n";
        
        file.close();
        std::cout << "Exported to " << filename << std::endl;
    }
};

int main() {
    std::string hl7_message = 
        "MSH|^~\\&|EPIC|EPICADT|SMS|SMSADT|199912271408|CHARRIS|ADT^A04|1817457|D|2.5|\n"
        "PID||0493575^^^2^ID 1|454721||DOE^JOHN^^^^|DOE^JOHN^^^^|19480203|M||B|254 MYSTREET AVE^^MYTOWN^OH^44123^USA||(216)123-4567|||M|NON|400003403~1129086|\n"
        "NK1||ROE^MARIE^^^^|SPO||(216)123-4567||EC|||||||||||||||||||||||||||\n"
        "PV1||O|168 ~219~C~PMA^^^^^^^^^||||277^ALLEN MYLASTNAME^BONNIE^^^^|||||||||| ||2688684|||||||||||||||||||||||||199912271408||||||002376853\n";
    std::cout << "HL7 Parser - Healthcare Message Parser\n" << std::endl;
    HL7Parser parser;
    parser.parse(hl7_message);
    parser.validate();
    parser.print();
    parser.export_json("hl7_output.json");
    std::cout << "\n=== USAGE EXAMPLE ===" << std::endl;
    std::cout << "Patient ID: " << parser.get_patient_id() << std::endl;
    std::cout << "Patient Name: " << parser.get_patient_name() << std::endl;
    std::cout << "Message Type: " << parser.get_message_type() << std::endl;
    
    return 0;
}