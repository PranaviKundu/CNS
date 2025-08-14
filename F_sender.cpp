// sender.cpp
#include <iostream>
#include <string>
#include <bitset>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

// Generate 12-bit Hamming Code from 8-bit data
string generateHammingCode(string data) {
    string hamming(12, '0');
    int j = 0;

    // Insert data bits into the Hamming code at non-parity positions
    for (int i = 0; i < 12; ++i) {
        if (i != 0 && i != 1 && i != 3 && i != 7) {
            hamming[i] = data[j++];
        }
    }

    // Calculate parity bits
    int parity_pos[] = {0, 1, 3, 7};
    for (int p : parity_pos) {
        int parity = 0;
        for (int i = 0; i < 12; ++i) {
            if (((i + 1) & (p + 1)) && i != p) {
                parity ^= (hamming[i] - '0');
            }
        }
        hamming[p] = parity + '0';
    }

    return hamming;
}

int main() {
    char ch;
    cout << "Enter first character of your name: ";
    cin >> ch;

    bitset<8> ascii(ch);
    string data = ascii.to_string();
    string codeword = generateHammingCode(data);

    cout << "Original ASCII in binary: " << data << endl;
    cout << "Generated Hamming Code: " << codeword << endl;

    // Setup TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = inet_addr("127.0.0.40");

    // Connect to server
    if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        close(sock);
        return 1;
    }

    // Simulate transmission error: flip 4th bit (index 3)
    codeword[3] = (codeword[3] == '0') ? '1' : '0';

    send(sock, codeword.c_str(), codeword.size(), 0);
    close(sock);

    cout << "Codeword sent to receiver.\n";
    return 0;
}
