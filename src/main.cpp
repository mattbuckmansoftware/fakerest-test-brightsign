#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include "data_retriever.h"
#include "data_handler.h"


void showHelp() {
    std::cout << "Usage: program <endpoint> [options; defaults to \"all\"]\n"
              << "Options:\n"
              << "  -h             Show this [H]elp message\n"
              << "  -y             Retrieve [Y]ears of life, average per city\n"
              << "  -f             Retrieve [F]riend count, average per city\n"
              << "  -m             Retrieve user with [M]ost friend count, per city\n"
              << "  -n             Retrieve most common first [N]ame, across all cities\n"
              << "  -r             Retrieve most common [R]ecreational hobby, across all friends in all cities\n";
}

int main(int argc, char* argv[]) {
    // Confirm minimum args have been received
    if (argc < 2) {
        std::cerr << "Error: Endpoint is required.\n";
        showHelp();
        return 1;
    }

    // First argument is always the endpoint
    std::string endpoint = argv[1];
    std::unordered_map<std::string, std::string> options;
    std::vector<std::string> flags = {"-h", "-y", "-f", "-m", "-n", "-r"};

    // Parse remaining arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h") {
            showHelp();
            return 0;
        } else if (std::find(flags.begin(), flags.end(), arg) != flags.end()) {
            options[arg] = "true";
        } else {
            std::cerr << "Error: Unknown option " << arg << "\n";
            return 1;
        }
    }
    
    // Pull data from endpoint
    int max_retries = 3;
    int retries = 0;
    std::string read_buffer;
    for(retries = 0; retries < max_retries; ++retries){
        //Pull the data
        if(DataRetriever::pullRESTData(endpoint, read_buffer)){
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    if(retries >= max_retries){
        std::cerr << "Exceeded retry limit; JSON data could not be pulled." << std::endl;
        return 2;
    }

    //Parse JSON
    DataHandler parser{};
    parser.parseJSON(read_buffer);

    //Print Output
    std::cout << parser.toJSONString(options) << std::endl;

    return 0;
}
