// Sample C++ program
#include <cpr/cpr.h> // How to install cpr?
#include <iostream>

int main() {

    std::string nwc_api_key = "NWC_MbxcI4LPORqaFc95oZUK4loa5rPJKy";
    std::string nwc_url_recentFlights = "https://newsky.app/api/airline-api/flights/recent";

    auto response = cpr::Get(
        cpr::Url{nwc_url_recentFlights},
        cpr::Header{{"Authorization", "Bearer " + nwc_api_key}}
    );

    std::cout << "Status code: " << response.status_code << std::endl;
    std::cout << "Response: " << response.text << std::endl;

    std::cout << "Hello, world!" << std::endl;
    return 0;
}
