#include <iostream>
#include <iomanip>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <vector>

#define PORT 5555
#define BUFFER_SIZE 1024
#define NEIGHBOR_TIMEOUT 30

using namespace std;

struct Kaimins {
    string ip;
    string mac;
    time_t pedejaAktivitatesLaiks;
};

int main() {
    int serveraSokets;
    struct sockaddr_in serveraAdrese, klientaAdrese;
    socklen_t klientaAdresesGarums = sizeof(klientaAdrese);
    char buf[BUFFER_SIZE];

    // Inicialize servera soketu
    serveraSokets = socket(AF_INET, SOCK_DGRAM, 0);
    if (serveraSokets == -1) {
        perror("Kluda, izveidojot soketu");
        exit(EXIT_FAILURE);
    }

    memset(&serveraAdrese, 0, sizeof(serveraAdrese));
    serveraAdrese.sin_family = AF_INET;
    serveraAdrese.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraAdrese.sin_port = htons(PORT);

    // Piesaistit soketu
    if (bind(serveraSokets, (struct sockaddr*)&serveraAdrese, sizeof(serveraAdrese)) == -1) {
        perror("Kluda, piesaistot soketu");
        close(serveraSokets);
        exit(EXIT_FAILURE);
    }

    vector<Kaimins> kaimini;

    while (true) {
        // Saņemt datus no klientiem
        ssize_t izlasitiBaiti = recvfrom(serveraSokets, buf, BUFFER_SIZE, 0,
                                         (struct sockaddr*)&klientaAdrese, &klientaAdresesGarums);

        if (izlasitiBaiti > 0) {
            buf[izlasitiBaiti] = '\0';

            // Izgut IP un MAC adresi no sanemta ziņojuma
            string ip = inet_ntoa(klientaAdrese.sin_addr);
            string mac(buf);

            // Pārbaudīt, vai kaimiņš jau ir sarakstā
            auto atrasts = find_if(kaimini.begin(), kaimini.end(),
                [ip](const Kaimins& k) { return k.ip == ip; });

            if (atrasts != kaimini.end()) {
                // Atjaunot pedējo aktīvā laika punktu
                atrasts->pedejaAktivitatesLaiks = time(nullptr);
            } else {
                // Pievienot jaunu kaimiņu sarakstam
                kaimini.push_back({ip, mac, time(nullptr)});
            }
        }

        // Izņemt neaktīvus kaimiņus
        kaimini.erase(remove_if(kaimini.begin(), kaimini.end(),
            [](const Kaimins& k) { return (time(nullptr) - k.pedejaAktivitatesLaiks) > NEIGHBOR_TIMEOUT; }),
            kaimini.end());

        // Pagaidīt pirms nākamās iterācijas
        sleep(1);
    }

    close(serveraSokets);

    return 0;
}
