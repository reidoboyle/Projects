// hw2_ReidOBoyle.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Native DNS Resolver
// Reid OBoyle
// CSCE 612 Netowrks and DP
// Spring 2022

#include "pch.h"
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#define MAX_DNS_LEN 512 // largest valid UDP packet 
#define MAX_ATTEMPTS 3 

/* flags */
#define DNS_QUERY (0 << 15) /* 0 = query; 1 = response */
#define DNS_RESPONSE (1 << 15)
#define DNS_STDQUERY (0 << 11) /* opcode - 4 bits */
#define DNS_AA (1 << 10) /* authoritative answer */
#define DNS_TC (1 << 9) /* truncated */
#define DNS_RD (1 << 8) /* recursion desired */
#define DNS_RA (1 << 7) /* recursion available */ 

/* query classes */
#define DNS_INET 1 
class buffer_ {
public:
    void reset()
    {
        pos = 0;
        memset(p, 0, 512);
    }
    char* p = new char[512];
    int pos = 0;
};
#pragma pack(push,1) // sets struct padding/alignment to 1 byte
class QueryHeader {
public:
    USHORT qType;
    USHORT qClass;
};
class FixedDNSheader {
public:
    USHORT ID;
    USHORT flags;
    USHORT questions;
    USHORT answers;
    USHORT authority;
    USHORT additional;
};
class DNSanswerHdr {
public:
    u_short type;
    u_short class_;
    u_int TTL;
    u_short len;
};
#pragma pack(pop) // restores old packing 

int countDots(char* pt)
{
    int oc = 0;
    int increments = 0;
    while (strlen(pt) !=0)
    {
        if (*pt == '.')
        {
            oc++;
        }
        pt++;
        increments++;
    }
    pt -= increments;
    return oc;
}
char* reverseFormat(char* ip)
{
    char seg1[5];char seg2[5];char seg3[5];char seg4[5];
    int sz1 = 0; int sz2 = 0; int sz3 = 0;int sz4 = 0;

    char* dot = strchr(ip, '.');
    sz1 = int(dot - ip);
    memcpy(seg1, ip, sz1);
    seg1[sz1] = 0;
    ip = dot+1;

    dot = strchr(ip, '.');
    sz2 = int(dot - ip);
    memcpy(seg2, ip,sz2);
    seg2[sz2] = 0;
    ip = dot+1;

    dot = strchr(ip, '.');
    sz3 = int(dot - ip);
    memcpy(seg3, ip,sz3);
    seg3[sz3] = 0;
    ip = dot+1;

    char* end = strchr(ip, '\0');
    sz4 = int(end -ip);
    memcpy(seg4, ip,sz4);
    seg4[sz4] = 0;

    char* reversed = (char*)malloc(34);
    sprintf(reversed, "%s.%s.%s.%s.in-addr.arpa", seg4, seg3, seg2, seg1);
    
    return reversed;
}

void makeDNSquestion(char* buf,char* host)
{
    int i = 0;
    char* dot = strchr(host, '.');
    int len = 0;
    while (dot != NULL)
    {
        len = dot - host;
        buf[i++] = len;
        memcpy(buf + i, host, len);
        i += len;
        host = dot + 1;
        dot = strchr(host, '.');
    }
    dot = strchr(host, '\0');
    buf[i++] = int(dot - host);
    memcpy(buf+i, host, dot - host);
    i += int(dot - host);
    buf[i] = 0;

}
int extractAns(char* ans,char* recbuf,int pktsize,buffer_* pBuf,char* occ)
{
    int ans_len = 0;
    int sz = 0;
    int jumps = 0;
    bool compressed = false;
    while (ans != NULL)
    {
        if ((unsigned char)*ans >= 0xC0)
        {
            // compressed
            compressed = true;
            jumps += 1;
            if (jumps > 10)
            {
                printf("++ invalid record: jump loop\n");
                return -1;
            }
            //check for truncated offset;
            if ((int)*(ans+1) == 0)
            {
                printf("++ invalid record: truncated jump offset\n");
                return -1;
            }
            int jump_off = (((unsigned char)*ans & 0x3F) << 8) + (unsigned char)*(ans + 1);
            if (jump_off > pktsize)
            {
                printf("++ invalid record: jump beyond packet boundary\n");
                return -1;
            }
            if (occ[jump_off] == '\x1')
            {
                printf("++invalid record: jump into fixed DNS header\n");
                return -1;
            }
            ans = recbuf + jump_off;
            continue;
        }
        else
        {
            //not compressed
            sz = (int)*ans;
            if (sz == 0)
            {
                break;
            }
            ans++;
            if (strlen(ans) < sz)
            {
                printf("++ invalid record: truncated name\n");
                return -1;
            }

            memcpy(pBuf->p+pBuf->pos, ans, sz);
            pBuf->pos += sz;

            ans += sz;
            ans_len += sz + 1;
            if ((int)*ans != 0)
            {
                memcpy(pBuf->p+pBuf->pos, ".",1);
                pBuf->pos++;
            }

        }

    }
    if (compressed)
    {
        return 2;
    }
    else
    {
        return ans_len+1;
    }
    
}

int main(int argc, char * argv[])
{

    if (argc != 3)
    {
        printf("Incorrect number of arguments, please use this format: arg1 = lookup string, arg2 = DNS Server IP\n");
        return 0;
    }
    char* lookup = argv[1];
    char* dnsServer = argv[2];
    u_short type = 12;
    srand(time(NULL));
    u_short txid = rand() % 1000;
    if (countDots(dnsServer) != 3)
    {
        printf("DNS Server is not a valid IPv4 address\n");
        return 0;
    }

    // decide query type
    char* query;
    DWORD in_addr = inet_addr(lookup);
    if (in_addr == INADDR_NONE)
    {
        type = 1;
        query = lookup;
    }
    else
    {
        // network byte order translation
        if (countDots(lookup) != 3)
        {
            printf("Lookup Ip is not a valid IPv4 Address\n");
            return 0;
        }
        query = reverseFormat(lookup);
    }


    printf("Lookup\t: %s\n", lookup);
    printf("Query\t: %s, type %d, TXID 0x%.4X\n",query,type,txid);
    printf("Server\t: %s\n", dnsServer);
    printf("******************************** \n");

    // start Winsock and create socket
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        printf("WSAStartup Error %d\n", WSAGetLastError());
        WSACleanup();
        exit(-1);
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM,0);
    if (sock == INVALID_SOCKET)
    {
        printf("socket() error %d\n", WSAGetLastError());
        WSACleanup();
        exit(-1);
    }



    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(0);

    if (bind(sock, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR)
    {
        printf("Bind error %d\n", WSAGetLastError());
        WSACleanup();
        exit(-1);
    }
    // successful binding
    struct sockaddr_in remote;
    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_addr.S_un.S_addr = inet_addr(dnsServer); // server’s IP
    remote.sin_port = htons(53); // DNS port on server

    // create buffer
    int pkt_size = strlen(query) + 2 + sizeof(FixedDNSheader) + sizeof(QueryHeader);
    char* packet = new char[MAX_DNS_LEN];
    char* buf = packet;

    FixedDNSheader* fdh = (FixedDNSheader*)packet;
    QueryHeader* qh = (QueryHeader*)(packet + pkt_size - sizeof(QueryHeader));
    // fixed field initialization
    fdh->ID = htons(txid);
    fdh->flags = htons(DNS_QUERY | DNS_RD);
    fdh->questions = htons(1);
    fdh->answers = htons(0);
    fdh->authority = htons(0);
    fdh->additional = htons(0);

    qh->qType = htons(type);
    qh->qClass = htons(DNS_INET);
    makeDNSquestion((char*)(fdh + 1), query);
    int attempt = 0;
    
    clock_t tm = clock();
    if(sendto(sock,packet, pkt_size, 0, (struct sockaddr*)&remote, sizeof(remote)) == SOCKET_ERROR)
    {
        // handle errors 
        printf("socket error %d\n", WSAGetLastError());
        WSACleanup();
        exit(-1);
    }


    char recbuf[MAX_DNS_LEN];
    char occupied[512] = { 0 };
    struct sockaddr_in response;
    int r_len = sizeof(response);
    int count = 0;
    while (count++ < MAX_ATTEMPTS)
    {

        printf("Attempt %d with %d bytes... ", attempt, pkt_size);
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        

        struct fd_set fd;
        FD_ZERO(&fd);
        FD_SET(sock, &fd);
        int available = select(0, &fd, NULL, NULL, &timeout);
        int brec = 0;
        if (available > 0)
        {
            //tm = clock();
            brec = recvfrom(sock, recbuf, MAX_DNS_LEN, 0, (struct sockaddr*)&response, &(r_len));
            
            tm = clock() - tm;
            double end_time = (double(tm)/CLOCKS_PER_SEC)*1000;
            if (brec == SOCKET_ERROR)
            {
                printf("socket error %d\n", WSAGetLastError());
                WSACleanup();
                exit(-1);
            }
            if (response.sin_addr.S_un.S_addr != remote.sin_addr.S_un.S_addr || response.sin_port != remote.sin_port)
            {
                // bogus reply complain
                printf("Bad reply\n");
                WSACleanup();
                exit(-1);
            }
            printf("response in %f ms with %i bytes\n", end_time, brec);
            if (recbuf[3] == -17)
            {
                printf("\t++ invalid reply: packet is junk\n");
                return 0;
            }
            recbuf[brec] = 0;
            if (brec < sizeof(FixedDNSheader))
            {
                printf("\t++ invalid reply: packet smaller than fixed DNS header\n");
                return 0;
            }
            FixedDNSheader* fdh = (FixedDNSheader*)recbuf;
            memset(occupied, 1, sizeof(FixedDNSheader));
            printf("\tTXID 0x%.4X,flags 0x%.4X, questions %d, answers %d, authority %d,additional %d\n", ntohs(fdh->ID), ntohs(fdh->flags), ntohs(fdh->questions), ntohs(fdh->answers), ntohs(fdh->authority), ntohs(fdh->additional));
            if (ntohs(fdh->ID) != txid)
            {
                printf("\t++ invalid reply: TXID Mismatch, sent 0x%.4X, recieved 0x%.4X\n", txid, (ntohs(fdh->ID)));
                return 0;
            }
            int offset = sizeof(FixedDNSheader);
            USHORT rcode = ntohs(fdh->flags) & 0x000F;
            if (rcode == 0)
            {
                printf("\tsucceeded with Rcode = %d\n", rcode);
            }
            else
            {
                printf("\tfailed with Rcode = %d\n", rcode);
                break;
            }
            buffer_* pBuf = new buffer_;
            printf("\t------------ [questions] ----------\n");
            USHORT numQuestions = ntohs(fdh->questions);
            for (int i = 0; i < numQuestions; i++)
            {
                pBuf->reset();
                char* question = recbuf + offset;
                printf("\t\t");
                
                int name_len = extractAns(question, recbuf,brec,pBuf,occupied);
                if (name_len < 0) { return 0; }
                printf("%s", pBuf->p);
                offset += name_len;

                QueryHeader* qhd = (QueryHeader*)(recbuf + offset);
                memset(occupied+offset, 1, sizeof(QueryHeader));
                offset += sizeof(QueryHeader);
                printf(" type %d class %d\n",ntohs(qhd->qType), ntohs(qhd->qClass));
            }

            USHORT nAnswers = ntohs(fdh->answers);
            USHORT nAuth = ntohs(fdh->authority);
            USHORT nAdd = ntohs(fdh->additional);
            //printf("%d,%d\n", nAuth, nAdd);

            for (int i = 0; i < nAnswers+nAuth+nAdd; i++)
            {
                pBuf->reset();
                if (offset >= brec)
                {
                    printf("\t\t++ invalid section: not enough records\n");
                    return 0;
                }
                if (i == 0)
                {
                    printf("\t------------ [answers] ------------\n");
                }
                else if(i == nAnswers && nAuth >0)
                {
                    printf("\t------------ [authority] ------------\n");
                }
                else if (i == nAuth+nAnswers &&nAdd > 0)
                {
                    printf("\t------------ [additional] ------------\n");
                }
                char* ans = recbuf + offset;
                printf("\t\t");
                int name_len = extractAns(ans, recbuf,brec,pBuf,occupied);
                if (name_len < 0) { return 0; }
                //printf("%s", pBuf->p);
                offset += name_len;
                if (offset + sizeof(DNSanswerHdr) > brec)
                {
                    printf("++ invalid record: truncated RR answer header\n");
                    return 0;
                }
                DNSanswerHdr* dah = (DNSanswerHdr*)(recbuf + offset);
                memset(occupied + offset, 1, sizeof(DNSanswerHdr));
                offset += sizeof(DNSanswerHdr);
                if (ntohs(dah->len) + offset > brec)
                {
                    printf("++ invalid record: RR value length stretches the answer beyond packet\n");
                    return 0;
                }
                USHORT type = ntohs(dah->type);

                struct in_addr* answer = (struct in_addr*)(recbuf + offset);
                switch (type)
                {
                case 1:
                    printf("%s", pBuf->p);
                    printf(" A ");
                    printf("%d.%d.%d.%d ", answer->S_un.S_un_b.s_b1, answer->S_un.S_un_b.s_b2, answer->S_un.S_un_b.s_b3, answer->S_un.S_un_b.s_b4);
                    break;
                case 2:
                    //printf(" NS ");
                    memcpy(pBuf->p+pBuf->pos, " NS ", 4);
                    pBuf->pos += 4;
                    ans = recbuf + offset;
                    name_len = extractAns(ans, recbuf,brec,pBuf,occupied);
                    if (name_len < 0) { return 0; }
                    printf("%s ", pBuf->p);
                    break;
                case 5:
                    //printf(" CNAME ");
                    memcpy(pBuf->p + pBuf->pos, " CNAME ", 7);
                    pBuf->pos += 7;
                    ans = recbuf + offset;
                    name_len = extractAns(ans, recbuf,brec,pBuf,occupied);
                    if (name_len < 0) { return 0; }
                    printf("%s ", pBuf->p);
                    break;
                case 12:
                    //printf(" PTR ");
                    memcpy(pBuf->p + pBuf->pos, " PTR ", 5);
                    pBuf->pos += 5;
                    ans = recbuf + offset;
                    name_len = extractAns(ans, recbuf,brec,pBuf,occupied);
                    if (name_len < 0) { return 0; }
                    printf("%s ",pBuf->p);
                    break;
                default:
                    //printf(" invalid response type\n");
                    continue;
                }
                
                printf("TTL = %d\n", ntohl(dah->TTL));
                offset += ntohs(dah->len);

            }

            break;
        }
        else
        {
            printf("timeout in %d ms\n",timeout.tv_sec+timeout.tv_usec);
        }
        attempt += 1;

    }
    WSACleanup();
    return 0;
}
