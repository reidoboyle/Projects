// Homework4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include "Router.h"
#define IP_HDR_SIZE 20 /* RFC 791 */
#define ICMP_HDR_SIZE 8 /* RFC 792 */
/* max payload size of an ICMP message originated in the program */
#define MAX_SIZE 65200
/* max size of an IP datagram */
#define MAX_ICMP_SIZE (MAX_SIZE + ICMP_HDR_SIZE)
/* the returned ICMP message will most likely include only 8 bytes
* of the original message plus the IP header (as per RFC 792); however,
* longer replies (e.g., 68 bytes) are possible */
#define MAX_REPLY_SIZE (IP_HDR_SIZE + ICMP_HDR_SIZE + MAX_ICMP_SIZE)
/* ICMP packet types */
#define ICMP_ECHO_REPLY 0
#define ICMP_DEST_UNREACH 3
#define ICMP_TTL_EXPIRED 11
#define ICMP_ECHO_REQUEST 8
/* remember the current packing state */
#pragma pack (push)
#pragma pack (1)
/* define the IP header (20 bytes) */
class IPHeader {
public:
    u_char h_len : 4; /* lower 4 bits: length of the header in dwords */
    u_char version : 4; /* upper 4 bits: version of IP, i.e., 4 */
    u_char tos; /* type of service (TOS), ignore */
    u_short len; /* length of packet */
    u_short ident; /* unique identifier */
    u_short flags; /* flags together with fragment offset - 16 bits */
    u_char ttl; /* time to live */
    u_char proto; /* protocol number (6=TCP, 17=UDP, ICMP =1) */
    u_short checksum; /* IP header checksum */
    u_long source_ip;
    u_long dest_ip;
};

/* define the ICMP header (8 bytes) */
class ICMPHeader {
public:
    u_char type; /* ICMP packet type */
    u_char code; /* type subcode */
    u_short checksum; /* checksum of the ICMP */
    u_short id; /* application-specific ID */
    u_short seq; /* application-specific sequence */
};
/* now restore the previous packing state */
#pragma pack (pop)

/*
* ======================================================================
* ip_checksum: compute Internet checksums
*
* Returns the checksum. No errors possible.
*
* ======================================================================
*/
u_short ip_checksum(u_short* buffer, int size)
{
    u_long cksum = 0;

    /* sum all the words together, adding the final byte if size is odd */
    while (size > 1)
    {
        cksum += *buffer++;
        size -= sizeof(u_short);
    }

    if (size)
    {
        cksum += *(u_char*)buffer;
    }
    /* add carry bits to lower u_short word */
    cksum = (cksum >> 16) + (cksum & 0xffff);

    /* return a bitwise complement of the resulting mishmash */
    return (u_short)(~cksum);
}

void DNSthread(Router* r)
{
    struct in_addr ip;
    ip.S_un.S_addr = r->intIP;
    r->ip = inet_ntoa(ip);

    struct hostent* ht = gethostbyaddr(inet_ntoa(ip), 4, AF_INET);
    if (ht != nullptr)
    {
        r->host = ht->h_name;
    }
    else
    {
       r->noDNS = true;
    }

}
int send(char * target,SOCKET * sock,struct sockaddr_in* remote,int i)
{
   int ttl = i + 1;
   if (setsockopt(*sock, IPPROTO_IP, IP_TTL, (const char*)&ttl, sizeof(ttl)) == SOCKET_ERROR)
   {
        printf("setsockopt failed with% d\n", WSAGetLastError());
        closesocket(*sock);
        // some cleanup
        exit(-1);
    }
    // buffer for the ICMP header
    char* send_buf = new char[ICMP_HDR_SIZE]; /* IP header is not present here */
    ICMPHeader* icmp = (ICMPHeader*)send_buf;
    // set up the echo request
    // no need to flip the byte order since fields are 1 byte each
    icmp->type = ICMP_ECHO_REQUEST;
    icmp->code = 0;
    //setup ID/SEQ feilds
    icmp->id = (u_short)GetCurrentProcessId();
    icmp->seq = i + 1;
    icmp->checksum = 0;
    /* calculate the checksum */
    int packet_size = sizeof(ICMPHeader); // 8 bytes
    icmp->checksum = ip_checksum((u_short*)send_buf, packet_size);

    int pktsize = sizeof(ICMPHeader);
    if (sendto(*sock, send_buf, pktsize, 0, (struct sockaddr*)remote, sizeof(*remote)) == SOCKET_ERROR)
    {
        printf("failed sendto with %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    delete[]send_buf;
}
int main(int argc, char* argv[])
{
    char* target = argv[1];
    if (argc != 2)
    {
        printf("Incorrect number of arguments, please enter a single destination hostname or IP");
        return 0;
    }
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);
    // start Winsock and create socket
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        printf("WSAStartup Error %d\n", WSAGetLastError());
        WSACleanup();
        exit(-1);
    }
    SOCKET sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock == INVALID_SOCKET)
    {
        printf("Unable to create a raw socket: error %d\n", WSAGetLastError());
        WSACleanup();
        exit(-1);
    }
    std::vector<Router> routes;
    for (int i = 0; i < 30; i++)
    {
        Router r = Router(i);
        routes.push_back(r);
    }

    struct sockaddr_in remote;
    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    DWORD ip_addr = inet_addr(target);
    if (ip_addr == INADDR_NONE)
    {
        struct hostent* host = gethostbyname(target);
        if (host == NULL)
        {
            printf("target invalid\n");
            return 0;
        }
        memcpy((char*)&(remote.sin_addr), host->h_addr, host->h_length);
    }
    else
    {
        remote.sin_addr.S_un.S_addr = ip_addr;
    }
    printf("Tracerouting to %s\n", inet_ntoa(remote.sin_addr));
    clock_t start_time = clock();

    for (int i = 0; i < 30; i++)
    {
        routes[i].start_c = clock();
        send(target, &sock, &remote, i);
    }
   


    // wait until eventquit
    HANDLE socketRecieveReady = CreateEvent(NULL, false, false, NULL);
    WSAEventSelect(sock, socketRecieveReady, FD_READ);

    // start timer for round trip for each packet
    clock_t timer = clock();

    struct sockaddr_in response;
    int r_len = sizeof(response);
    struct timeval timeout;
    timeout.tv_sec = int(1);
    timeout.tv_usec = int(fmod(1, 1) * 1000);
    struct fd_set fd;
    FD_ZERO(&fd);
    FD_SET(sock, &fd);
    bool echoRec = false;
    
    HANDLE* dnsHandles = new HANDLE[30];
    int dnsLookups = 0;

    int collected = 0;
    int needed = 30;
    int r;
    while (true)
    {
        if (collected >= needed)
        {
            break;
        }
        if (double(clock() - start_time) / CLOCKS_PER_SEC > 20)
        {
            break;
        }
        r = WaitForSingleObject(socketRecieveReady, 500);

        if (r == WAIT_TIMEOUT)
        {
            //resend the packet(s) we are waiting on
            for (int i = 0; i < needed; i++)
            {
                if (routes[i].recieved || routes[i].attempt >= 3)
                {
                    if (routes[i].attempt >= 3 && !routes[i].failed)
                    {
                        routes[i].failed = true;
                        collected += 1;
                    }
                    continue;
                }
                else
                {
                    double time = double(clock() - routes[i].start_c) / CLOCKS_PER_SEC*1000;
                    if (time > routes[i].timeout)
                    {
                        // resend
                        routes[i].timeout = routes[i].timeout*2;
                        send(target, &sock, &remote, i);
                        routes[i].attempt += 1;
                    }
                }
            }

        }
        else if (r == WAIT_FAILED)
        {
            // print error message
            printf("Failed socket recieve with %d\n", WSAGetLastError());
            WSACleanup();
            return -1;
        }
        else
        {
            // good extract the response
            char* recBuf = new char[1000];/* this buffer starts with an IP header */
            IPHeader* router_ip_hdr = (IPHeader*)recBuf;
            ICMPHeader* router_icmp_hdr = (ICMPHeader*)(router_ip_hdr + 1);
            IPHeader* orig_ip_hdr = (IPHeader*)(router_icmp_hdr + 1);
            ICMPHeader* orig_icmp_hdr = (ICMPHeader*)(orig_ip_hdr + 1);
            
            int brec = recvfrom(sock, recBuf, 1000, 0, (struct sockaddr*)&response, &(r_len));
            if (brec == SOCKET_ERROR) {
                printf("Failed recvfrom with %d\n", WSAGetLastError());
                WSACleanup();
                return -1;
            }
            
            // check if this is TTL_expired; make sure packet size >= 56 bytes
            if (router_icmp_hdr->type == ICMP_TTL_EXPIRED && router_icmp_hdr->code == 0)
            {
                if (orig_ip_hdr->proto == 1 /*ICMP*/)
                {
                    // check if process ID matches
                    if (orig_icmp_hdr->id == GetCurrentProcessId())
                    {
                        // take router_ip_hdr->source_ip and
                        //struct in_addr ip;
                        //ip.S_un.S_addr = router_ip_hdr->source_ip;
                        routes[orig_icmp_hdr->seq - 1].intIP = router_ip_hdr->source_ip;
                        // initiate a DNS lookup
                        //char* ip_str = inet_ntoa(*ip);
                        //routes[orig_icmp_hdr->seq - 1].ip = inet_ntoa(ip);
                        //struct hostent* router = gethostbyaddr(inet_ntoa(ip), 4, AF_INET);

                        dnsHandles[dnsLookups] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DNSthread,&(routes[orig_icmp_hdr->seq-1]), 0, NULL);
                        dnsLookups++;
                      
                        routes[orig_icmp_hdr->seq-1].recieved = true;
                        routes[orig_icmp_hdr->seq-1].time = double(clock() - routes[orig_icmp_hdr->seq - 1].start_c) / CLOCKS_PER_SEC*1000;
                        collected += 1;
                        //printf("%d %s (%s) ...", icmp->seq,router->h_name,inet_ntoa(ip));
                    }
                }
            }
            else if(router_icmp_hdr->type == ICMP_ECHO_REPLY)
            {
                if (!echoRec)
                {
                    collected += 1;
                }
                echoRec = true;
                routes[int(router_icmp_hdr->seq - 1)].recieved = true;;
                if (router_icmp_hdr->seq < needed)
                {
                    needed = router_icmp_hdr->seq;
                }
                routes[int(router_icmp_hdr->seq - 1)].echo = true;
                routes[int(router_icmp_hdr->seq - 1)].time = double(clock() - routes[int(router_icmp_hdr->seq - 1)].start_c) / CLOCKS_PER_SEC*1000;

                routes[router_icmp_hdr->seq-1].intIP = router_ip_hdr->source_ip;
                dnsHandles[dnsLookups] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DNSthread, &(routes[router_icmp_hdr->seq - 1]), 0, NULL);
                dnsLookups++;
                //printf("%d\n", router_icmp_hdr->seq - 1);
            }


            delete[]recBuf;

        }
    }
    for (int i = 0; i < dnsLookups; i++)
    {
        WaitForSingleObject(dnsHandles[i], INFINITE);
        CloseHandle(dnsHandles[i]);
    }
    // pprint stats
    for (int i = 0; i < 30; i++)
    {
        if (routes[i].failed )
        {
            printf("%d\t*\n", routes[i].seq);
        }
        else if (routes[i].noDNS)
        {
            printf("%d\t%s ", routes[i].seq, "<no DNS entry>");
            std::cout << " (" << routes[i].ip << ") ";
            printf("%.3f ms (%d)\n", routes[i].time, routes[i].attempt);

        }
        else
        {
            printf("%d\t", routes[i].seq);
            std::cout << routes[i].host << " (" << routes[i].ip << ") ";
            printf("%.3f ms (%d)\n", routes[i].time, routes[i].attempt);
        }
        if (routes[i].echo)
        {
            break;
        }

    }

    double total = double(clock()-start_time) / CLOCKS_PER_SEC * 1000.0;
    printf("Total execution time: %f ms", total);
    WSACleanup();

    return 0;
}
