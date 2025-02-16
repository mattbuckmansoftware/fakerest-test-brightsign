#pragma once

#include <string>
#include <iostream>
#include <unordered_map>
#include <simdjson.h>
#include "logger.h"

//This struct holds the data used for city-based calculations
struct CityData {
    //Constructors
    CityData(const std::string& name):
        name_{name},
        summed_years_of_life_{0},
        summed_friend_count_{0},
        most_friends_count_{0},
        people_count_{0}
    {

    };
    CityData() = default;

    //Mutator for updating city's data when new inidividual is processed
    void addIndividualData(size_t individual_age, size_t individual_friend_count){
        summed_years_of_life_ += individual_age;
        summed_friend_count_ += individual_friend_count;

        if(individual_friend_count > most_friends_count_){
            most_friends_count_ = individual_friend_count;
        }

        ++people_count_;
    };

    //Calculate and return average age
    float getAverageAge() const {
        return static_cast<float>(summed_years_of_life_) / people_count_;
    }

    //Calculate and return average friends
    float getAverageFriendCount() const {
        return static_cast<float>(summed_friend_count_) / people_count_;
    }

    //Return max friend count
    float getMostFriendsCount() const {
        return most_friends_count_;
    }

    //Stream insertion operator (for debugging)
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

class DataHandler
{
public:
    DataHandler();

    //This function parses the relevant data from the given string
    int parseJSON(const std::string& read_buffer);

    //This function composes the relevant data into a JSON string, in accordance with the given options
    std::string toJSONString(const std::unordered_map<std::string, std::string>& options);

private:
    //Helper functions
    static bool extractAge(simdjson::ondemand::document& json, uint64_t& age);
    static bool extractName(simdjson::ondemand::document& json, std::string& name_str);
    static bool extractCity(simdjson::ondemand::document& json, std::string& city_str);
    static bool extractFriendsAndHobbies(simdjson::ondemand::document& json, size_t& friend_count,
                                  std::vector<std::pair<std::string, std::vector<std::string>>>& friends_hobbies);
    void updateNames(const std::string& name_str);
    void updateCities(const std::string& city_str, uint64_t age, size_t friend_count);
    void updateHobbies(const std::vector<std::pair<std::string, std::vector<std::string>>>& friends_hobbies);

    //Member variables
    std::unordered_map<std::string, CityData> cities_;
    std::unordered_map<std::string, size_t> names_;
    std::unordered_map<std::string, size_t> hobbies_;
};