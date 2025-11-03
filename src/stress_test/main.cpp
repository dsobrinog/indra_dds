#include <iostream>

// Executive Test

#include "core/executive.h"
#include "core/dds_module.h"

#include "common_defs.h"

#include <unordered_map>

#include <nlohmann/json.hpp>
#include <fstream>

#include <cstdio>
#include <iostream>

Executive* _exec; 
using json = nlohmann::json;

void removeFileIfExists(const std::string& path)
{
    if (std::remove(path.c_str()) == 0)
        std::cout << "Deleted file: " << path << "\n";
    else
        std::cout << "File not found or could not delete: " << path << "\n";
}




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
    std::string config_path = "/workspaces/fastdds/config/default.json"; // default
    if (args.find("config") != args.end())
    {
        config_path = args.at("config");
    }
    else
    {
        std::cerr << "No --config argument provided. Using default: " << config_path << std::endl;
    }

    std::string logFile_path = "/workspaces/fastdds/logs/default.log"; // default
    if (args.find("logFile") != args.end())
    {
        logFile_path = args.at("logFile");
        removeFileIfExists(logFile_path);
    }
    else
    {
        std::cerr << "No --logFile argument provided. Using default: " << logFile_path << std::endl;
    }
    
    // Load configuration
    auto config = load_config(config_path);
    // std::cout << "Config content:\n" << config.dump(4) << std::endl;

    //// -----
    //// CONFIG LOAD
    //// -----

    // =========== Load fields ================
    std::string name          = config.value("name", "Default test");
    int mode                  = config.value("mode", 0);
    int messageCount          = config.value("messageCount", 100);
    int test                  = config.value("test", 25);
    bool manual_process       = config.value("manual_process", true);
    bool loan                 = config.value("loan", false);
    int num_listeners         = config.value("listeners", 0);
    
    // =========== Output ================
    std::cout << "\n=== EXECUTIVE*** : Start Simulation ===\n";
    std::cout << "  Config from  : " << config_path << std::endl;
    std::cout << "  Test Name    : " << name << std::endl;
    std::cout << "  Mode         : " << ToString((TestMode)mode) << std::endl;
    std::cout << "  MessageCount : " << messageCount << std::endl;
    std::cout << "  Test Number  : " << test << std::endl;
    std::cout << "  Log File     : " << logFile_path << std::endl;
    std::cout << "  Manual Mode  : " << std::boolalpha << manual_process << std::endl;
    std::cout << "  Loan Mode    : " << std::boolalpha << loan << std::endl;
    std::cout << "  Num Listeners: " << num_listeners << std::endl;
    std::cout << "=========================================\n";

    //// -----
    //// EXEC LOGIC
    //// -----

    // Start Simulation
    _exec = new Executive;
    test_config _test_config;
    _test_config.name = name;
    _test_config.test_mode = (TestMode) mode;
    _test_config.message_count = messageCount;
    _test_config.test_count = test;
    _test_config.logFile = logFile_path;
    _test_config.manual_mode = manual_process;
    _test_config.loan_mode = loan;
    _test_config.listener_number = num_listeners;
    
    cl_dds* moduleDDS = new cl_dds(_test_config, _exec);

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
