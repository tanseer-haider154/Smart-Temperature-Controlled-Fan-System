// ============================================================
// Smart Temperature-Controlled Fan System
// C++ Desktop Monitoring Application (Console Dashboard)
// Reads live data from Arduino over Serial Port and displays
// it in a colorful, live-updating table with a battery-style
// visual indicator and estimated RPM.
// ============================================================

#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

using namespace std;

HANDLE hSerial;
const float MAX_RPM = 6000.0;   // Estimated max RPM (edit if motor's rated RPM is known)

bool openSerialPort(const char* portName) {
    hSerial = CreateFileA(portName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) return false;

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    GetCommState(hSerial, &dcbSerialParams);
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    SetCommState(hSerial, &dcbSerialParams);

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);
    return true;
}

string readSerialLine() {
    string result = "";
    char c;
    DWORD bytesRead;
    while (true) {
        if (ReadFile(hSerial, &c, 1, &bytesRead, NULL) && bytesRead > 0) {
            if (c == '\n') break;
            if (c != '\r') result += c;
        }
    }
    return result;
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

vector<string> splitByPipe(const string& line) {
    vector<string> parts;
    stringstream ss(line);
    string item;
    while (getline(ss, item, '|')) {
        size_t start = item.find_first_not_of(" \t");
        size_t end = item.find_last_not_of(" \t");
        if (start != string::npos)
            parts.push_back(item.substr(start, end - start + 1));
        else
            parts.push_back("");
    }
    return parts;
}

int getStatusColor(const string& status) {
    if (status == "NORMAL") return 10;
    if (status == "HIGH") return 14;
    if (status == "MAX") return 12;
    if (status == "SHUTDOWN!") return 4;
    return 7;
}

string makeBatteryBar(float percent, int totalBlocks = 20) {
    int filled = (int)((percent / 100.0) * totalBlocks);
    if (filled > totalBlocks) filled = totalBlocks;
    if (filled < 0) filled = 0;
    string bar = "[";
    for (int i = 0; i < filled; i++) bar += "|";
    for (int i = filled; i < totalBlocks; i++) bar += " ";
    bar += "]-";
    return bar;
}

int main() {
    string portInput;
    cout << "=== Smart Cooling Fan Monitor ===" << endl;
    cout << "Arduino ka COM port number likhein (jaise 4 agar COM4 hai): ";
    cin >> portInput;
    string portName = "\\\\.\\COM" + portInput;

    if (!openSerialPort(portName.c_str())) {
        cout << "Error: Port khul nahi saka. COM number Device Manager se check karein." << endl;
        return 1;
    }

    cout << "Connected! Data ana shuru ho raha hai...\n";
    Sleep(2000);

    while (true) {
        string line = readSerialLine();
        if (line.empty()) continue;
        if (line[0] == '=' || line[0] == '-' || line.find("ADC") != string::npos) continue;

        vector<string> parts = splitByPipe(line);
        if (parts.size() < 5) continue;

        int adcValue = 0;
        float voltage = 0, temperature = 0, fanPercent = 0;
        string status = parts[4];

        stringstream(parts[0]) >> adcValue;
        stringstream(parts[1]) >> voltage;
        stringstream(parts[2]) >> temperature;
        stringstream(parts[3]) >> fanPercent;

        system("cls");
        setColor(11);
        cout << "======================================================" << endl;
        cout << "         SMART TEMPERATURE-CONTROLLED FAN SYSTEM        " << endl;
        cout << "======================================================" << endl;

        setColor(9);
        cout << left << setw(22) << "ADC Value" << ": " << adcValue << endl;
        setColor(13);
        cout << left << setw(22) << "Voltage" << ": " << voltage << " V" << endl;
        setColor(14);
        cout << left << setw(22) << "Temperature" << ": " << temperature << " C" << endl;

        setColor(getStatusColor(status));
        cout << left << setw(22) << "Fan Speed" << ": " << fanPercent << " %" << endl;
        cout << left << setw(22) << "  Level" << ": " << makeBatteryBar(fanPercent) << endl;

        setColor(10);
        float rpmNow = (fanPercent / 100.0) * MAX_RPM;
        cout << left << setw(22) << "Estimated RPM" << ": " << rpmNow << endl;

        setColor(7);
        cout << "------------------------------------------------------" << endl;
        setColor(getStatusColor(status));
        cout << ">> Status: " << status << endl;
        setColor(11);
        cout << "======================================================" << endl;
        setColor(7);

        Sleep(1000);
    }

    CloseHandle(hSerial);
    return 0;
}
