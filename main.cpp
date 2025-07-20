// Sample C++ program
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

int main() {

    std::string nwc_api_key = "NWC_MbxcI4LPORqaFc95oZUK4loa5rPJKy";
    //std::string nwc_url_recentFlights = "https://newsky.app/api/airline-api/flights/recent";
    //std::string nw_url_recentFlights = "https://newsky.app/api/airline/64207611e254b8ce5d3d540a/aircraft/list";
    std::string nwc_url_aircrafts = "https://newsky.app/api/airline/66af4fd3655bfa7f31107e19/aircraft/list";
    

    //nlohmann::json body_json;
    //body_json["date"] = "20250125";

    auto response = cpr::Post(
        cpr::Url{nwc_url_aircrafts},
        cpr::Header{
            {"Authorization", "Bearer " + nwc_api_key},
            {"Accept", "application/json"}
            //{"Content-Type", "application/json"}
        }
        //cpr::Body{body_json.dump()} // Attach the JSON body
    );

    std::cout << "Status code: " << response.status_code << std::endl;
    std::cout << "Response written to 'response.json file'. " << std::endl;

    // Parse and display JSON
    /*
    try {
        auto json = nlohmann::json::parse(response.text);
        std::cout << "Total Results: " << json["totalResults"] << std::endl;
        std::cout << "Flights:" << std::endl;
        for (const auto& flight : json["results"]) {
            std::cout << flight.dump(2) << std::endl; // Pretty print each flight
        }
    } catch (const std::exception& e) {
        std::cout << "Failed to parse JSON: " << e.what() << std::endl;
        std::cout << "Raw response: " << response.text << std::endl;
    }
    */
   // Write the response to a file
   try {
        auto json = nlohmann::json::parse(response.text);

        // Write JSON to a file
        std::ofstream outFile("response.json");
        outFile << json.dump(2); // Pretty print with indent=2
        outFile.close();

        std::cout << "JSON response written to response.json" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Failed to parse JSON: " << e.what() << std::endl;
        std::cout << "Raw response: " << response.text << std::endl;
    }
    

    std::cout << "Hello, world!" << std::endl;
    return 0;
}
