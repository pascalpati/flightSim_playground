// Sample C++ program
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "SimConnect.h"

// 1. Define the data structure
struct PitotHeatData {
    int pitotHeatOn; // BOOL (0 = off, 1 = on)
};

enum PITOT_HEAT_DEFINE_ID {
    PITOT_HEAT_DEFINITION = 100
};

enum PITOT_HEAT_REQUEST_ID {
    PITOT_HEAT_REQUEST = 100
};

int callApiAndSaveResponse(const std::string& api_key, const std::string& url) {
    auto response = cpr::Post(
        cpr::Url{url},
        cpr::Header{
            {"Authorization", "Bearer " + api_key},
            {"Accept", "application/json"}
        }
    );

    std::cout << "Status code: " << response.status_code << std::endl;

    // Parse and write the response to a file
    try {
        auto json = nlohmann::json::parse(response.text);
        std::ofstream outFile("response.json");
        outFile << json.dump(2);
        outFile.close();
        std::cout << "JSON response written to response.json" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Failed to parse JSON: " << e.what() << std::endl;
        std::cout << "Raw response: " << response.text << std::endl;
    }

    return response.status_code;
}

HRESULT simConnect_Open(HANDLE* hSimConnect)
{
    // Parameters for SimConnect_Open
    const char* szName = "flightSim_playground";
    HWND hWnd = 0; // Use 0 if you don't have a window handle
    DWORD UserEventWin32 = 0;
    HANDLE hSimConnectLocal = nullptr;

    HRESULT hr = SimConnect_Open(
        &hSimConnectLocal, // Pointer to handle
        szName,            // Application name
        hWnd,              // Window handle (0 if not using Win32 events)
        UserEventWin32,    // Win32 event ID (0 if not using)
        0,                 // Reserved, must be 0
        0                  // Flags, must be 0
    );

    #ifdef SIMCONNECT_DEBUG
        std::cout << "[SimConnect_Open] Handle after open: " << hSimConnectLocal << std::endl;
    #endif

    if (SUCCEEDED(hr)) {
        *hSimConnect = hSimConnectLocal;
    } else {
        *hSimConnect = nullptr;
    }
    return hr;
}

// Function to request a simulator system state
HRESULT simConnect_RequestSystemState(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID requestID, const char* szState)
{
    std::cout << "[SimConnect_RequestSystemState] Handle: " << hSimConnect
              << ", State: " << szState << std::endl;

    HRESULT hr = SimConnect_RequestSystemState(
        hSimConnect,     // SimConnect handle
        requestID,       // Client-defined request ID
        szState          // System state string ("AircraftLoaded", "DialogMode", "FlightLoaded", "FlightPlan", "Sim")
    );

    if (SUCCEEDED(hr)) {
        std::cout << "SimConnect_RequestSystemState succeeded for state: " << szState << std::endl;
    } else {
        std::cout << "SimConnect_RequestSystemState failed for state: " << szState << ". HRESULT: " << hr << std::endl;
    }
    return hr;
}

void printSystemStateResult(SIMCONNECT_RECV* pData) {
    auto* pState = reinterpret_cast<SIMCONNECT_RECV_SYSTEM_STATE*>(pData);
    std::cout << "System state \"" << pState->szString << "\" value: " << pState->dwInteger << std::endl;
    if (strlen(pState->szString) > 0)
        std::cout << "Returned string: " << pState->szString << std::endl;
}

HRESULT simConnect_Close(HANDLE hSimConnect)
{
    // Closes the SimConnect connection and releases resources
    #ifdef SIMCONNECT_DEBUG
    std::cout << "[SimConnect_Close] Handle before close: " << hSimConnect << std::endl;
    #endif
    // Check if the handle is valid before closing
    if (hSimConnect != nullptr) {
        return SimConnect_Close(hSimConnect);
    }
    return S_OK; // Nothing to close
}

int main() {

    HANDLE hSimConnect = nullptr;

    std::cout << "Beginning of the program!" << std::endl;
    
    std::string nwc_api_key = "NWC_MbxcI4LPORqaFc95oZUK4loa5rPJKy";
    //std::string nwc_url_recentFlights = "https://newsky.app/api/airline-api/flights/recent";
    //std::string nw_url_recentFlights = "https://newsky.app/api/airline/64207611e254b8ce5d3d540a/aircraft/list";
    std::string nwc_url_aircrafts = "https://newsky.app/api/airline/66af4fd3655bfa7f31107e19/aircraft/list";
    
    HRESULT hr = simConnect_Open(&hSimConnect);
    if (SUCCEEDED(hr)) {
        std::cout << "SimConnect opened successfully!" << std::endl;

        // ...inside main(), after simConnect_Open...
        HRESULT hrDef = SimConnect_AddToDataDefinition(
            hSimConnect,
            PITOT_HEAT_DEFINITION,
            "PITOT HEAT", "Bool",
            SIMCONNECT_DATATYPE_INT32,
            0, 0
        );

        // Request pitot heat status from user aircraft (object ID 0)
        SimConnect_RequestDataOnSimObject(
            hSimConnect,
            PITOT_HEAT_REQUEST,
            PITOT_HEAT_DEFINITION,
            0, // User aircraft
            SIMCONNECT_PERIOD_SECOND
        );

         // Request and print the "Sim" state every 5 seconds
        for (int i = 0; i < 10; ++i) { // Example: 10 times
            HRESULT hrSim = simConnect_RequestSystemState(hSimConnect, 1, "Sim");
            if (SUCCEEDED(hrSim)) {
                std::cout << "\"Sim\" state request succeeded." << std::endl;
            } else {
                std::cout << "\"Sim\" state request failed. HRESULT: " << hrSim << std::endl;
            }

            // Event loop: print only when pitot heat status changes
            int lastPitotHeat = -1; // Initialize to an invalid value
            bool running = true;
            while (running) {
                SIMCONNECT_RECV* pData = nullptr;
                DWORD cbData = 0;
                HRESULT hrRecv = SimConnect_GetNextDispatch(hSimConnect, &pData, &cbData);
                if (SUCCEEDED(hrRecv) && pData) {
                    if (pData->dwID == SIMCONNECT_RECV_ID_SIMOBJECT_DATA) {
                        auto* pObj = reinterpret_cast<SIMCONNECT_RECV_SIMOBJECT_DATA*>(pData);
                        if (pObj->dwRequestID == PITOT_HEAT_REQUEST) {
                            PitotHeatData* data = reinterpret_cast<PitotHeatData*>(&pObj->dwData);
                            if (data->pitotHeatOn != lastPitotHeat) {
                                std::cout << "Pitot heat status changed: " << (data->pitotHeatOn ? "ON" : "OFF") << std::endl;
                                lastPitotHeat = data->pitotHeatOn;
                            }
                        }
                    }
                    if (pData->dwID == SIMCONNECT_RECV_ID_QUIT) {
                        running = false;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            // Wait for the next iteration
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }

    } else {
        std::cout << "Failed to open SimConnect. HRESULT: " << hr << std::endl;
    }

    // Close SimConnect connection before exiting
    HRESULT hrClose = simConnect_Close(hSimConnect);
    if (SUCCEEDED(hrClose)) {
        std::cout << "SimConnect closed successfully!" << std::endl;
    } else {
        std::cout << "Failed to close SimConnect. HRESULT: " << hrClose << std::endl;
    }


    // Call the API and save the response
    /*
    int status_code = callApiAndSaveResponse(nwc_api_key, nwc_url_aircrafts);
    if (status_code != 200) {
        std::cout << "Error calling API. Status code: " << status_code << std::endl;
    } else {
        std::cout << "API call successful!" << std::endl;
    }
    */

    std::cout << "End of the program!" << std::endl;
    return 0;
}
