#pragma once

#include <string>
#include <curl/curl.h>
#include <iostream>
#include "logger.h"

namespace DataRetriever {

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
            Logger::getInstance() << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
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
            Logger::getInstance() << "HTTP error: " << http_code << std::endl;
            return false;
        }
    } else {
        // CURL initialization failed
        Logger::getInstance() << "curl_easy_init() failed; check your system configuration." << std::endl;
        return false;
    }

    // If we've made it this far, the request was successful
    return true;
}

} // namespace DataRetriever
