#include "data_handler.h"

DataHandler::DataHandler() :
    cities_{},
    names_{},
    hobbies_{}
{

}

// Helper function to extract age from JSON
bool DataHandler::extractAge(simdjson::ondemand::document& json, uint64_t& age) {
    try {
        if (json["age"].type() == simdjson::ondemand::json_type::number) {
            // If age is a number, extract it directly
            auto error = json["age"].get(age);
            if (error) {
                Logger::getInstance() << "invalid age; skipping" << std::endl;
                return false;
            }
        } else if (json["age"].type() == simdjson::ondemand::json_type::string) {
            // If age is a string, convert it to a number
            std::string_view str_value;
            if (auto error = json["age"].get(str_value); error) {
                Logger::getInstance() << "Error getting age string: " << error << "; skipping" << std::endl;
                return false;
            }
            try {
                age = std::stoull(std::string(str_value));
            } catch (const std::exception& e) {
                Logger::getInstance() << "Error converting age string to uint64_t: " << e.what() << "; skipping" << std::endl;
                return false;
            }
        } else {
            Logger::getInstance() << "Could not parse age; skipping" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        Logger::getInstance() << "Error extracting age: " << e.what() << "; skipping" << std::endl;
        return false;
    }
    return true;
}

// Helper function to extract name from JSON
bool DataHandler::extractName(simdjson::ondemand::document& json, std::string& name_str) {
    try {
        if (json["name"].type() == simdjson::ondemand::json_type::string) {
            std::string_view str_value;
            if (auto error = json["name"].get(str_value); error) {
                Logger::getInstance() << "Error getting name string: " << error << "; skipping" << std::endl;
                return false;
            }
            name_str = std::string(str_value);
        } else {
            Logger::getInstance() << "could not parse name; skipping" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        Logger::getInstance() << "Error extracting name: " << e.what() << "; skipping" << std::endl;
        return false;
    }
    return true;
}

// Helper function to extract city from JSON
bool DataHandler::extractCity(simdjson::ondemand::document& json, std::string& city_str) {
    try {
        if (json["city"].type() == simdjson::ondemand::json_type::string) {
            std::string_view str_value;
            if (auto error = json["city"].get(str_value); error) {
                Logger::getInstance() << "Error getting city string: " << error << "; skipping" << std::endl;
                return false;
            }
            city_str = std::string(str_value);
        } else {
            Logger::getInstance() << "could not parse city; skipping" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        Logger::getInstance() << "Error extracting city: " << e.what() << "; skipping" << std::endl;
        return false;
    }
    return true;
}

// Helper function to extract friends and their hobbies from JSON
bool DataHandler::extractFriendsAndHobbies(simdjson::ondemand::document& json, size_t& friend_count,
                              std::vector<std::pair<std::string, std::vector<std::string>>>& friends_hobbies) {
    try {
        simdjson::ondemand::value friends = json["friends"];
        friend_count = friends.count_elements();

        for (auto friend_obj : friends) {
            std::string_view friend_name = friend_obj["name"].get_string();
            std::vector<std::string> friend_hobbies;

            auto parsed_hobbies = friend_obj["hobbies"];
            for (auto hobby : parsed_hobbies) {   
                friend_hobbies.push_back(static_cast<std::string>(hobby.get_string().value()));
            }

            friends_hobbies.emplace_back(std::string(friend_name), friend_hobbies);
        }
    } catch (const std::exception& e) {
        Logger::getInstance() << "Error extracting friends and hobbies: " << e.what() << "; skipping" << std::endl;
        return false;
    }
    return true;
}

// Helper function to update the names map
void DataHandler::updateNames(const std::string& name_str) {
    if(names_.find(name_str) == names_.end()){
        names_.insert(std::make_pair(name_str, 0));
    }
    ++names_[name_str];
}

// Helper function to update the cities map
void DataHandler::updateCities(const std::string& city_str, uint64_t age, size_t friend_count) {
    if(cities_.find(city_str) == cities_.end()){
        cities_.insert(std::make_pair(city_str, CityData(city_str)));
    }
    cities_[city_str].addIndividualData(age, friend_count);
}

// Helper function to update the hobbies map
void DataHandler::updateHobbies(const std::vector<std::pair<std::string, std::vector<std::string>>>& friends_hobbies) {
    for (const auto& [friend_name, friend_hobbies] : friends_hobbies) {
        for (const auto& hobby : friend_hobbies) {
            if(hobbies_.find(hobby) == hobbies_.end()){
                hobbies_.emplace(hobby, 0);
            }
            ++hobbies_[hobby];
        }
    }
}

//This function parses the JSON from the read_buffer into the destination variables
int DataHandler::parseJSON(const std::string& read_buffer) {

    std::istringstream iss(read_buffer);
    std::string line;
    int valid_count = 0;
    int invalid_count = 0;

    // Process each line in the input buffer
    while (std::getline(iss, line)) {
        simdjson::ondemand::parser parser;
        simdjson::ondemand::document json;
        try {
            // Parse the JSON line
            json = parser.iterate(line);
        } catch (simdjson::simdjson_error& e) {
            Logger::getInstance() << "Invalid JSON: " << line.substr(0, 50) << "...; skipping" << std::endl;
            invalid_count++;
            continue;
        }

        // Temporary variables to store extracted data
        uint64_t age;
        std::string name_str;
        std::string city_str;
        size_t friend_count;
        std::vector<std::pair<std::string, std::vector<std::string>>> friends_hobbies;

        // Extract each piece of data, skipping the entire object if any extraction fails
        if (!extractAge(json, age) ||
            !extractName(json, name_str) ||
            !extractCity(json, city_str) ||
            !extractFriendsAndHobbies(json, friend_count, friends_hobbies)) {
            invalid_count++;
            continue;
        }

        // If all extractions were successful, update the reference arguments
        updateNames(name_str);
        updateCities(city_str, age, friend_count);
        updateHobbies(friends_hobbies);

        ++valid_count;
    }
    Logger::getInstance() << "Valid count: " << valid_count << ", invalid count: " << invalid_count << std::endl;

    return valid_count;
}

//This function returns the output JSON, according to requested options
std::string DataHandler::toJSONString(const std::unordered_map<std::string, std::string>& options) {
    const bool printAll = options.empty();
    auto shouldPrint = [&options, printAll](const std::string& opt) {
        return printAll || options.find(opt) != options.end();
    };

    std::ostringstream json;
    json << "{\n";

    // City data
    if (shouldPrint("-y") || shouldPrint("-f") || shouldPrint("-m")) {
        json << "\t\"city_data\": [\n";
        for (auto it = cities_.begin(); it != cities_.end(); ++it) {
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
            json << "\n\t\t\t}\n\t\t}" << (std::next(it) != cities_.end() ? ",\n" : "\n");
        }
        json << "\t],\n";
    }

    // Most common name
    if (shouldPrint("-n")) {
        auto maxName = std::max_element(names_.begin(), names_.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        if (maxName != names_.end()) {
            json << "\t\"most_common_name\": {\n\t\t\"name\": \"" << maxName->first 
                 << "\",\n\t\t\"count\": " << maxName->second << "\n\t},\n";
        }
    }

    // Most common hobby
    if (shouldPrint("-r")) {
        auto maxHobby = std::max_element(hobbies_.begin(), hobbies_.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        if (maxHobby != hobbies_.end()) {
            json << "\t\"most_common_hobby\": {\n\t\t\"hobby\": \"" << maxHobby->first 
                 << "\",\n\t\t\"count\": " << maxHobby->second << "\n\t},\n";
        }
    }

    json.seekp(-2, std::ios_base::end); // Remove trailing comma and newline
    json << "\n}";

    return json.str();
}