#include <vector>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <strings.h>
#include <netdb.h>

#include "types.h"


using namespace std;

// Function Prototype(s)
void * init (void * nothing);

// Vector containing all players in the current game
vector<player_matchmaking_t> players;


int main(int argc, char *argv[])
{
    long sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 4) {
       fprintf(stderr, "Usage: %s [hostname] [port] [clientname]\n", argv[0]);
       exit(0);
    }

    /*-- Connect ---------- */
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr,
            (char *)server->h_addr,
            server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    /* ---------- Connect --*/

    // Initialize a new player_matchmaking_t struct with default values,
    // then assign the appropriate values.
    player_matchmaking_t p = {{0, 0}, {0}, 0, 0, 0, false};
    strcpy(p.name, argv[3]);
    p.team = 1;
    p.role = 2;
    p.ready = true;

    if ((n = write(sockfd, &p, sizeof(p))) < 0)
        error("ERROR writing to socket");

    send_chat(sockfd, "Hello server!");
    pthread_t t;
    pthread_create(&t, NULL, init, (void*)sockfd);

    while (1) {
        header_t head;

        if ((n = recv_complete(sockfd, &head, sizeof(head), 0)) < 0)
            error("Disconnect");

        // [Jesse]: I'm probably going to change this to a switch statement
        // at some point in time.  It displeases me in its current state.
        if (head.type == MSG_PLAYER_UPDATE) {
            player_matchmaking_t p;
            p.head = head;
            n = recv_complete(sockfd, ((char*)&p) + sizeof(head), sizeof(p) - sizeof(head), 0);

            if (n < 0)
                error("ERROR reading from socket");

            printf("Player: %s\t"
                "Team: %d\t"
                "Role: %d\t"
                "Ready: %s\n",
                p.name, p.team,
                p.role, (p.ready ? "yes" : "no"));

            players.push_back(p);
        } else if (head.type == MSG_PLAYER_LEAVE) {
            player_matchmaking_t p;
            p.head = head;
            n = recv_complete(sockfd, ((char*)&p) + sizeof(head), sizeof(p) - sizeof(head), 0);

            if (n < 0)
                error("ERROR reading from socket");

            printf("Player Left: %s\n", p.name);

            // Remove player from our list.
            players.erase(std::remove(players.begin(), players.end(), p), players.end());
        } else if (head.type == MSG_MAPNAME) {
            //n = recv_complete(sockfd, ((char*)&m + sizeof(head)), sizeof(map_t) - sizeof(head), 0);
            char m[MAP_NAME_SIZE] = {0};
            if ((n = recv_complete(sockfd, m, MAP_NAME_SIZE, 0)) > 0)
                printf("Got map name: %s\n", m);
        } else if (head.type == MSG_CHAT) {
            printf("got msgchat size: %d\n", head.size);

            char m[head.size];

            n = recv_complete(sockfd, m, head.size, 0);
            m[n] = 0;

            if (n > 0)
                printf("message: %s\n", m);
        }
    }

    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);

    return 0;
}

