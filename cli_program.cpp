#include <iostream>
#include <iomanip>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5555
#define BUFFER_SIZE 1024

using namespace std;

int main() {
    int klientaSokets;
    struct sockaddr_in serveraAdrese;
    char buferis[BUFFER_SIZE];

    // Inicializēt klienta soketu
    klientaSokets = socket(AF_INET, SOCK_DGRAM, 0);
    if (klientaSokets == -1) {
        perror("Kļūda, izveidojot soketu");
        exit(EXIT_FAILURE);
    }

    memset(&serveraAdrese, 0, sizeof(serveraAdrese));
    serveraAdrese.sin_family = AF_INET;
    serveraAdrese.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraAdrese.sin_port = htons(PORT);

    // Sūtīt pieprasījumu uz serveri
    sendto(klientaSokets, "KaiminuPiecere", sizeof("KaiminuPiecere"), 0,
           (struct sockaddr*)&serveraAdrese, sizeof(serveraAdrese));

    // Saņemt un parādīt aktīvo kaimiņu sarakstu
    ssize_t izlasitiBaiti = recvfrom(klientaSokets, buferis, BUFFER_SIZE, 0, NULL, NULL);
    if (izlasitiBaiti > 0) {
        buferis[izlasitiBaiti] = '\0';
        cout << "Aktīvie kaimiņi:" << endl;
        cout << setw(15) << "IP adrese" << setw(18) << "MAC adrese" << endl;
        cout << "-------------------------" << endl;
        cout << buferis;
    } else {
        cout << "Nav aktīvu kaimiņu." << endl;
    }

    close(klientaSokets);

    return 0;
}
