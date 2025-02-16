#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <simdjson.h>
#include <unordered_map>
#include <fstream>
#include <thread>


void showHelp() {
    std::cout << "Usage: program <endpoint> [options]\n"
              << "Options:\n"
              << "  -h             Show this [H]elp message\n"
              << "  -y             Retrieve [Y]ears of life, average per city\n"
              << "  -f             Retrieve [F]riend count, average per city\n"
              << "  -m             Retrieve user with [M]ost friend count, per city\n"
              << "  -n             Retrieve most common first [N]ame, across all cities\n"
              << "  -r             Retrieve most common [R]ecreational hobby, across all friends in all cities\n";
}

// Callback function to write response data
static size_t writeCallback(void *contents, size_t size, size_t nmemb, std::string *s) {
    size_t newLength = size * nmemb;
    s->append((char*)contents, newLength);
    return newLength;
}

// Pull REST data
bool pullRESTData(const std::string& endpoint, std::string &results) {
    CURL *curl;
    CURLcode res;

    // Initialize a CURL session
    curl = curl_easy_init();
    if(curl) {
        // Set the URL to fetch
        curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
        // Set the callback function to handle the response data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        // Set the variable to store the response data
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &results);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors in the request execution
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return false;
        }

        // Get the HTTP response code
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        // Clean up the CURL session
        curl_easy_cleanup(curl);

        // Check for HTTP errors (4xx and 5xx status codes)
        if(http_code >= 400) {
            std::cerr << "HTTP error: " << http_code << std::endl;
            return false;
        }
    } else {
        // CURL initialization failed
        std::cerr << "curl_easy_init() failed; check your system configuration." << std::endl;
        return false;
    }

    // If we've made it this far, the request was successful
    return true;
}

struct CityData {
    CityData(const std::string& name):
        name_{name},
        summed_years_of_life_{0},
        summed_friend_count_{0},
        most_friends_count_{0},
        people_count_{0}
    {

    };
    CityData() = default;

    void addIndividualData(size_t individual_age, size_t individual_friend_count){
        summed_years_of_life_ += individual_age;
        summed_friend_count_ += individual_friend_count;

        if(individual_friend_count > most_friends_count_){
            most_friends_count_ = individual_friend_count;
        }

        ++people_count_;
    };

    float getAverageAge() const {
        return static_cast<float>(summed_years_of_life_) / people_count_;
    }

    float getAverageFriendCount() const {
        return static_cast<float>(summed_friend_count_) / people_count_;
    }

    float getMostFriendsCount() const {
        return most_friends_count_;
    }

    friend std::ostream& operator<<(std::ostream& os, const CityData& city) {
        os << "City: " << city.name_ << "\n"
           << "Average Age: " << city.getAverageAge() << "\n"
           << "Average Friend Count: " << city.getAverageFriendCount() << "\n"
           << "Most Friends Count: " << city.getMostFriendsCount() << "\n"
           << "Total People: " << city.people_count_ << "\n";
        return os;
    }

private:
    std::string name_;
    size_t summed_years_of_life_;
    size_t summed_friend_count_;
    size_t most_friends_count_;
    size_t people_count_;
};

void parseJSON(const std::string& read_buffer,
    std::unordered_map<std::string, CityData> &cities,
    std::unordered_map<std::string, size_t>& names,
    std::unordered_map<std::string, size_t>& hobbies) {

    std::istringstream iss(read_buffer);
    std::string line;
    int valid_count = 0;
    int invalid_count = 0;

    while (std::getline(iss, line)) {
        simdjson::ondemand::parser parser;
        simdjson::ondemand::document json;
        try {
            json = parser.iterate(line); //This does *not* validate the line
        } catch (simdjson::simdjson_error& e) {
            // Handle the error, then continue to the next line
            std::cerr << "Invalid JSON: " << line.substr(0, 50) << "...; skipping" << std::endl;
            invalid_count++;
            continue;
        }

        // Access fields in each JSON object
        uint64_t age;
        try{
            //If age is a number
            if (json["age"].type() == simdjson::ondemand::json_type::number) {
                auto error = json["age"].get(age);
                if (error) { 
                    std::cerr << "invalid age; skipping" << std::endl;
                    invalid_count++;
                    continue;
                }
            //If age is formatted as a string
            } else if (json["age"].type() == simdjson::ondemand::json_type::string) {
                std::string_view str_value;
                if (auto error = json["age"].get(str_value); error) {
                    std::cerr << "Error getting age string: " << error << "; skipping" << std::endl;
                    invalid_count++;
                    continue;
                } else {
                    try {
                        age = std::stoull(std::string(str_value));
                    } catch (const std::exception& e) {
                        std::cerr << "Error converting age string to uint64_t: " << e.what() << "; skipping" << std::endl;
                        invalid_count++;
                        continue;
                    }
                }
            } else {
                std::cerr << "Could not parse age; skipping" << std::endl;
                invalid_count++;
                continue;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error extracting age: " << e.what() << "; skipping" << std::endl;
            invalid_count++;
            continue;
        }

        std::string_view name;
        try{
            if (json["name"].type() == simdjson::ondemand::json_type::string) {
                std::string_view str_value;
                if (auto error = json["name"].get(str_value); error) {
                    std::cerr << "Error getting name string: " << error << "; skipping" << std::endl;
                    invalid_count++;
                    continue;
                }
                name = str_value;
            } else {
                std::cerr << "could not parse name; skipping" << std::endl;
                invalid_count++;
                continue;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error extracting name: " << e.what() << "; skipping" << std::endl;
            invalid_count++;
            continue;
        }

        //Update name counter
        std::string name_str{name};
        if(names.find(name_str) == names.end()){
            names.insert(std::make_pair(name_str, 0));
        }
        ++names[name_str];

        std::string city_str;
        try{
            std::string_view city;
            if (json["city"].type() == simdjson::ondemand::json_type::string) {
                std::string_view str_value;
                if (auto error = json["city"].get(str_value); error) {
                    std::cerr << "Error getting city string: " << error << "; skipping" << std::endl;
                    invalid_count++;
                    continue;
                }
                city = str_value;
                city_str = std::string(city);
            } else {
                std::cerr << "could not parse city; skipping" << std::endl;
                invalid_count++;
                continue;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error extracting name: " << e.what() << "; skipping" << std::endl;
            invalid_count++;
            continue;
        }

        simdjson::ondemand::value friends;
        try{
            friends = json["friends"];
        } catch (const std::exception& e) {
            std::cerr << "Error extracting friends: " << e.what() << "; skipping" << std::endl;
            invalid_count++;
            continue;
        }
        size_t friend_count = friends.count_elements();

        //Update city's data
        if(cities.find(city_str) == cities.end()){
            cities.insert(std::make_pair(city_str, CityData(city_str)));
        }
        cities[city_str].addIndividualData(age, friend_count);

        // Iterate through the "friends" array
        for (auto friend_obj : friends) {
            std::string_view friend_name = friend_obj["name"].get_string();

            // Iterate through the "hobbies" array
            auto parsed_hobbies = friend_obj["hobbies"];
            for (auto hobby : parsed_hobbies) {   
                std::string hobby_str = static_cast<std::string>(hobby.get_string().value());
          
                if(hobbies.find(hobby_str) == hobbies.end()){
                    hobbies.emplace(hobby_str, 0);
                }
                ++hobbies[hobby_str];
            }
        }
        ++valid_count;
    }
    std::cerr << "Valid count: " << valid_count << ", invalid count: " << invalid_count << std::endl;
}

void printOutput(const std::unordered_map<std::string, std::string>& options,
                 const std::unordered_map<std::string, CityData>& cities,
                 const std::unordered_map<std::string, size_t>& names,
                 const std::unordered_map<std::string, size_t>& hobbies) {
    
    const bool printAll = options.empty();
    auto shouldPrint = [&options, printAll](const std::string& opt) {
        return printAll || options.find(opt) != options.end();
    };

    std::ostringstream json;
    json << "{\n";

    // City data
    if (shouldPrint("-y") || shouldPrint("-f") || shouldPrint("-m")) {
        json << "\t\"city_data\": [\n";
        for (auto it = cities.begin(); it != cities.end(); ++it) {
            const auto& [city, data] = *it;
            json << "\t\t{\n\t\t\t\"city\": \"" << city << "\",\n\t\t\t\"data\": {\n";
            
            if (shouldPrint("-y")) {
                json << "\t\t\t\t\"average_age\": " << data.getAverageAge() << ",\n";
            }
            if (shouldPrint("-f")) {
                json << "\t\t\t\t\"average_friend_count\": " << data.getAverageFriendCount() << ",\n";
            }
            if (shouldPrint("-m")) {
                json << "\t\t\t\t\"most_friends_count\": " << data.getMostFriendsCount() << ",\n";
            }
            
            json.seekp(-2, std::ios_base::end); // Remove trailing comma and newline
            json << "\n\t\t\t}\n\t\t}" << (std::next(it) != cities.end() ? ",\n" : "\n");
        }
        json << "\t],\n";
    }

    // Most common name
    if (shouldPrint("-n")) {
        auto maxName = std::max_element(names.begin(), names.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        if (maxName != names.end()) {
            json << "\t\"most_common_name\": {\n\t\t\"name\": \"" << maxName->first 
                 << "\",\n\t\t\"count\": " << maxName->second << "\n\t},\n";
        }
    }

    // Most common hobby
    if (shouldPrint("-r")) {
        auto maxHobby = std::max_element(hobbies.begin(), hobbies.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        if (maxHobby != hobbies.end()) {
            json << "\t\"most_common_hobby\": {\n\t\t\"hobby\": \"" << maxHobby->first 
                 << "\",\n\t\t\"count\": " << maxHobby->second << "\n\t},\n";
        }
    }

    json.seekp(-2, std::ios_base::end); // Remove trailing comma and newline
    json << "\n}";

    std::cout << json.str() << std::endl;
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
        if(pullRESTData(endpoint, read_buffer)){
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    if(retries >= max_retries){
        std::cerr << "Exceeded retry limit" << std::endl;
        return 1;
    }

    //!!!Debug
    {
    std::ofstream outFile("curl_results");        
    if (outFile.is_open()) {
        outFile << read_buffer;
        outFile.close();
    }
    }

    //Parse JSON
    std::unordered_map<std::string, CityData> cities;
    std::unordered_map<std::string, size_t> names;
    std::unordered_map<std::string, size_t> hobbies;
    parseJSON(read_buffer, cities, names, hobbies);

    //!!!Debug
    {
        std::ofstream outFile("cities_results");        
        if (outFile.is_open()) {
            for(const auto& [city_name, city_data] : cities){
                outFile << "City: " << city_name << "\n" << city_data << "\n";
            }
            outFile.close();
        }
    }

    //!!!Debug
    {
        std::ofstream outFile("names_results");        
        if (outFile.is_open()) {
            for(const auto& [name, count] : names){
                outFile << "Name: " << name << "\nCount: " << count << "\n\n";
            }
            outFile.close();
        }
    }

    //!!!Debug
    {
        std::ofstream outFile("hobbies_results");        
        if (outFile.is_open()) {
            for(const auto& [hobby, count] : hobbies){
                outFile << "Hobby: " << hobby << "\nCount: " << count << "\n\n";
            }
            outFile.close();
        }
    }

    //Print city data
    printOutput(options, cities, names, hobbies);

    return 0;
}
