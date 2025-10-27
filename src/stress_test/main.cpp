#include <iostream>

// Executive Test
#include "core/executive.h"
#include "core/dds_module.h"

#include "common_defs.h"

#include <unordered_map>

#include <nlohmann/json.hpp>
#include <fstream>

Executive* _exec; 
using json = nlohmann::json;


// Simple helper to parse key-value arguments (--key value)
// Ej. ./exec_app --mode 1 --count 5000 --log logs/run1.log
std::unordered_map<std::string, std::string> parse_args(int argc, char* argv[])
{
    std::unordered_map<std::string, std::string> args;
    for (int i = 1; i < argc; ++i)
    {
        std::string key = argv[i];
        if (key.rfind("--", 0) == 0 && i + 1 < argc)
        {
            args[key.substr(2)] = argv[++i]; // skip next since it's value
        }
    }
    return args;
}

bool stringToBool(const std::string& str) {
    return str == "true" || str == "1";
}

const std::string path_to_config = "../config/";
// Helper function to load config
json load_config(const std::string& config_path = "config.json") {
    std::ifstream config_file(config_path);
    if (!config_file.is_open()) {
        std::cerr << "Warning: Config file not found, using defaults\n";
        return json::object();
    }
    
    try {
        return json::parse(config_file);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing config: " << e.what() << "\n";
        return json::object();
    }
}


int main(int argc, char* argv[])
{
    // Parse command-line arguments into key-value pairs
    auto args = parse_args(argc, argv);
    
    // Check if --config is provided
    std::string config_path = "../config/default.json"; // default
    if (args.find("config") != args.end())
    {
        config_path = args.at("config");
    }
    else
    {
        std::cerr << "No --config argument provided. Using default: " << config_path << std::endl;
    }
    
    // Load configuration
    auto config = load_config(config_path);

    //// -----
    //// CONFIG LOAD
    //// -----

    // =========== Load fields ================
    std::string name          = config.value("name", "Default test");
    int mode                  = config.value("mode", -1);
    int messageCount          = config.value("messageCount", 100);
    int test                  = config.value("test", 25);
    std::string logFile       = config.value("logFile", "default.log");
    bool manual_process       = config.value("manual_process", true);
    bool loan                = config.value("loan", false);
    
    // =========== Output ================
    std::cout << "\n=== EXECUTIVE*** : Start Simulation ===\n";
    std::cout << "  Config from  : " << config_path << std::endl;
    std::cout << "  Test Name    : " << name << std::endl;
    std::cout << "  Mode         : " << mode << std::endl;
    std::cout << "  MessageCount : " << messageCount << std::endl;
    std::cout << "  Test Number  : " << test << std::endl;
    std::cout << "  Log File     : " << logFile << std::endl;
    std::cout << "  Manual Mode  : " << std::boolalpha << manual_process << std::endl;
    std::cout << "  Loan Mode    : " << std::boolalpha << loan << std::endl;
    std::cout << "=========================================\n";

    //// -----
    //// EXEC LOGIC
    //// -----

    // Start Simulation
    _exec = new Executive;
    cl_dds* moduleDDS = new cl_dds(0, mode, _exec);
    moduleDDS->set_expected_messages(messageCount);
    moduleDDS->set_tests(test);
    moduleDDS->set_mode(manual_process);
    moduleDDS->set_loan(loan);

    _exec->add_module(moduleDDS);
    _exec->init();

    // Loop
    std::cout << "EXECUTIVE*** : Begining Execution..." << std::endl;
    while (_exec->in_execution())
    {
        _exec->execute_cycle();
    }

    
    // End
    _exec->end_simulation();
    std::cout << "EXECUTIVE*** : End Simulation..." << std::endl;
    delete _exec;
    return 0;
}
