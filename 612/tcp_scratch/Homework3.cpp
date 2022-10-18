// Homework3.cpp : This file contains the 'main' function. Program execution begins and ends there.
// CSCE 612 
// Reid OBoyle
// Homework 3 part 1

#include "pch.h"
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

// possible status codes from ss.Open, ss.Send, ss.Close
#define STATUS_OK 0 // no error
#define ALREADY_CONNECTED 1 // second call to ss.Open() without closing connection
#define NOT_CONNECTED 2 // call to ss.Send()/Close() without ss.Open()
#define INVALID_NAME 3 // ss.Open() with targetHost that has no DNS entry
#define FAILED_SEND 4 // sendto() failed in kernel
#define TIMEOUT 5 // timeout after all retx attempts are exhausted
#define FAILED_RECV 6 // recvfrom() failed in kernel
#define MAX_RTX 50  // maximum number of retransmissions

#define MAGIC_PROTOCOL 0x8311AA
#define MAGIC_PORT 22345
#define MAX_PKT_SIZE (1500-28)

#define FORWARD_PATH 0  // forward loss index
#define RETURN_PATH 1   // return loss index

#pragma pack(push,1) // sets struct padding/alignment to 1 byte
class Flags {
public:
    DWORD reserved : 5; // must be zero
    DWORD SYN : 1;
    DWORD ACK : 1;
    DWORD FIN : 1;
    DWORD magic : 24;
    Flags() { memset(this, 0, sizeof(*this)); magic = 8589738/*MAGIC_PROTOCOL*/; }
};
class LinkProperties {
public:
    // transfer parameters
    float RTT; // propagation RTT (in sec)
    float speed; // bottleneck bandwidth (in bits/sec)
    float pLoss[2]; // probability of loss in each direction
    DWORD bufferSize; // buffer size of emulated routers (in packets)
    LinkProperties() { memset(this, 0, sizeof(*this)); }
};
class SenderDataHeader {
public:
    Flags flags;
    DWORD seq; // must begin from 0
};
class SenderSynHeader {
public:
    SenderDataHeader sdh;
    LinkProperties lp;
};
class ReceiverHeader {
public:
    Flags flags;
    DWORD recvWnd; // receiver window for flow control (in pkts)
    DWORD ackSeq; // ack value = next expected sequence
};
#pragma pack(pop) // restores old packing
class Parameters {
public:
    HANDLE mutex;
    LPCRITICAL_SECTION cs;
    HANDLE empty;
    HANDLE full;
    HANDLE SocketRecieveReady;
    HANDLE finished;
    HANDLE eventQuit;
    int timedOut = 0;
    int senderBase = 0;
    int next = 0;
    int fastRet = 0;
    int winSize = 1;
    double speed = 0.0;
    double estRTT;
    int previousBase = 0;
    int dupACK = 0;
};
DWORD WINAPI statsThread(LPVOID pParam);

class SenderSocket {
public:
    // default constructor
    SenderSocket() {
        // start Winsock and create socket
        if (WSAStartup(wVersionRequested, &wsaData) != 0)
        {
            printf("WSAStartup Error %d\n", WSAGetLastError());
            WSACleanup();
            exit(-1);
        }
        sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock == INVALID_SOCKET)
        {
            printf("socket() error %d\n", WSAGetLastError());
            WSACleanup();
            exit(-1);
        }
        int kernelBuffer = 0x20e6;
        char* sendBuf = new char[kernelBuffer];
        if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, sendBuf, sizeof(int)) == SOCKET_ERROR);
        char* recBuf = new char[kernelBuffer];
        if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, recBuf, sizeof(int)) == SOCKET_ERROR);

        start_time = clock();
        dynamicRTO = RTT();
    };
    int Open(char* target,int port,int window, LinkProperties* lp,Parameters* pp,UINT64 bbs)
    {
        p = pp;
        W = window;
        if (bbs % (MAX_PKT_SIZE - sizeof(SenderDataHeader)) == 0)
        {
            lastBase = int(bbs / (MAX_PKT_SIZE - sizeof(SenderDataHeader)));
        }
        else
        {
            lastBase = int(bbs / (MAX_PKT_SIZE - sizeof(SenderDataHeader))) + 1;
        }
        for (int i = 0; i < W; i++)
        {
            char* pt = new char[MAX_PKT_SIZE];
            Packet pack = Packet::Packet(pt, MAX_PKT_SIZE, 2, 0);
            pkts.push_back(pack);
        }
        if (connected)
        {
            return ALREADY_CONNECTED;
        }

        rto = max(1, 2 * lp->RTT);
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;
        local.sin_port = htons(0);
        // bind socket
        if (bind(sock, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR)
        {
            printf("Bind error %d\n", WSAGetLastError());
            WSACleanup();
            exit(-1);
        }
        // successful binding do DNS
        
        memset(&remote, 0, sizeof(remote));
        remote.sin_family = AF_INET;
        DWORD ip_addr = inet_addr(target);
        if (ip_addr == INADDR_NONE)
        {
            struct hostent*  host =  gethostbyname(target);
            
            if (host == NULL)
            {
                double elapsed = (double(clock() - start_time) / CLOCKS_PER_SEC);
                printf("[%.3f] --> target %s is invalid\n", elapsed, target);
                return INVALID_NAME;
                //WSACleanup();
                //exit(-1);
            }
            memcpy((char*)&(remote.sin_addr), host->h_addr, host->h_length);
        }
        else
        {
            remote.sin_addr.S_un.S_addr = ip_addr;
        }
        remote.sin_port = htons(MAGIC_PORT);
        // create the buffer
        char* pkt = new char[MAX_PKT_SIZE];
        int pktsize = sizeof(SenderSynHeader);
        SenderSynHeader* ssh = (SenderSynHeader*)pkt;
        ssh->sdh.flags.reserved = 0b00000;
        ssh->sdh.flags.SYN = 0b1;
        ssh->sdh.flags.ACK = 0b0;
        ssh->sdh.flags.FIN = 0b0;
        ssh->sdh.flags.magic = 8589738;
        ssh->sdh.seq = 0;
        ssh->lp = *lp;
        char* recBuf = new char[MAX_PKT_SIZE];

        for (int i = 0; i < 3; i++)
        {
            //clock_t tm = clock() - start_time;
            double elapsed = (double(clock() - start_time) / CLOCKS_PER_SEC);
            //printf("[%.3f] --> SYN 0 (attempt %d of 3, RTO %.3f) to %s \n",elapsed, i+1, rto, inet_ntoa(remote.sin_addr));
            clock_t openRTT = clock();
            if (sendto(sock, pkt, pktsize, 0, (struct sockaddr*)&remote, sizeof(remote)) == SOCKET_ERROR)
            {
                elapsed = (double(clock() - start_time) / CLOCKS_PER_SEC);
                printf("[%.3f] --> failed sendto with %d\n",elapsed, WSAGetLastError());
                WSACleanup();
                return FAILED_SEND;
            }
            struct sockaddr_in response;
            int r_len = sizeof(response);
            struct timeval timeout;
            timeout.tv_sec = int(rto);
            timeout.tv_usec = int(fmod(rto,1)*1000);
            struct fd_set fd;
            FD_ZERO(&fd);
            FD_SET(sock,&fd);
            int available = select(0, &fd, NULL,&fd, &timeout);
            
            if (available > 0)
            {
                int brec = recvfrom(sock, recBuf, MAX_PKT_SIZE, 0, (struct sockaddr*)&response, &(r_len));
                if (brec == SOCKET_ERROR)
                {
                    elapsed = (double(clock() - start_time) / CLOCKS_PER_SEC);
                    printf("[%.3f] <-- failed recvfrom with %d\n",elapsed, WSAGetLastError());
                    WSACleanup();
                    return FAILED_SEND;
                }
                double initEstRTT = double(clock() - openRTT) / CLOCKS_PER_SEC;
                dynamicRTO.setEstimatedRTT(initEstRTT);
                p->estRTT = initEstRTT;
                ReceiverHeader* rh = (ReceiverHeader*)recBuf;
                WSAEventSelect(sock, p->SocketRecieveReady, FD_READ);

                lastReleased = min(W, rh->recvWnd);
                ReleaseSemaphore(p->empty, lastReleased,NULL);
                WSAEventSelect(sock, p->SocketRecieveReady, FD_READ);
                //TESTING THIS OUT
                
                //elapsed = (double(clock() - start_time) / CLOCKS_PER_SEC);
                //rto = elapsed * 3;
                //dynamicRTO.setEstimatedRTT(rto);
                //printf("[%.3f] <-- SYN-ACK %d window %d; setting initial RTO to %.3f\n",elapsed, rh->ackSeq, rh->recvWnd, rto);
                connected = true;
                return 0;

            }
            
        }
        return TIMEOUT;
    }
    RTT getRTO()
    {
        return dynamicRTO;
    }
    int Close(Parameters* p)
    {
        if (!connected)
        {
            return NOT_CONNECTED;
        }
        char* pkt = new char[sizeof(SenderDataHeader)];
        int pktsize = sizeof(SenderDataHeader);
        SenderDataHeader* sdh = (SenderDataHeader*)pkt;
        sdh->flags.reserved = 0b00000;
        sdh->flags.SYN = 0b0;
        sdh->flags.ACK = 0b0;
        sdh->flags.FIN = 0b1;
        sdh->flags.magic = 8589738;
        sdh->seq = senderBase;
        char* recBuf = new char[MAX_PKT_SIZE];
        for (int i = 0; i < 5; i++)
        {
            double elapsed = (double(clock() - start_time) / CLOCKS_PER_SEC);
            if (sendto(sock, pkt, pktsize, 0, (struct sockaddr*)&remote, sizeof(remote)) == SOCKET_ERROR)
            {
                elapsed = (double(clock() - start_time) / CLOCKS_PER_SEC);
                printf("[%.3f] --> failed sendto with %d\n", elapsed, WSAGetLastError());
                WSACleanup();
                return FAILED_SEND;
            }
            struct sockaddr_in response;
            int r_len = sizeof(response);
            struct timeval timeout;
            timeout.tv_sec = int(1);
            timeout.tv_usec = int(fmod(1, 1) * 1000);
            struct fd_set fd;
            FD_ZERO(&fd);
            FD_SET(sock, &fd);
            int available = select(0, &fd, NULL, NULL, &timeout);

            if (available > 0)
            {
                int brec = recvfrom(sock, recBuf, MAX_PKT_SIZE, 0, (struct sockaddr*)&response, &(r_len));
                if (brec == SOCKET_ERROR)
                {
                    elapsed = (double(clock() - start_time) / CLOCKS_PER_SEC);
                    printf("[%.3f] <-- failed recvfrom with %d\n", elapsed, WSAGetLastError());
                    WSACleanup();
                    return FAILED_SEND;
                }
                ReceiverHeader* rh = (ReceiverHeader*)recBuf;
                if (rh->flags.FIN == 1 && rh->flags.ACK == 1)
                {
                    elapsed = (double(clock() - start_time) / CLOCKS_PER_SEC);
                    printf("[%.2f] <-- FIN-ACK %d window %X\n", elapsed, rh->ackSeq, rh->recvWnd);
                    connected = false;
                    return 0;
                }
                else
                {
                    continue;
                }
            }
        }
        return TIMEOUT;
    }
    

    void Producer( unsigned char* buffer,int bytes,Parameters* p,int seq,bool Fin=false)
    {
        HANDLE arr[] = {p->eventQuit,p->empty };
        WaitForMultipleObjects(2, arr, false, INFINITE);
        UINT64 off = 0;
        //while (off < total)
        //{
        //int bytes = min(bytes, MAX_PKT_SIZE - sizeof(SenderDataHeader));
        int slot = seq % W;
        char* pt = pkts[slot].pkt;// new char[sizeof(SenderDataHeader) + bytes];
        int pktSize = bytes + sizeof(SenderDataHeader);
        SenderDataHeader* sdh = (SenderDataHeader*)pt;
        
        sdh->flags.reserved = 0b00000;
        sdh->flags.SYN = 0b0;
        sdh->flags.ACK = 0b0;
        sdh->flags.FIN = 0b0;
        sdh->flags.magic = 8589738;
        sdh->seq = seq;
        memset(pt + sizeof(SenderDataHeader), 0, MAX_PKT_SIZE - sizeof(SenderDataHeader));
        if (bytes < (MAX_PKT_SIZE-sizeof(SenderDataHeader))) 
        { 
            //sdh->flags.FIN = 0b1;
            memcpy(pt + sizeof(SenderDataHeader), buffer, bytes);
            Packet pack = Packet::Packet(pt, sizeof(SenderDataHeader) + bytes, 2, seq);
            pkts[slot] = pack;
        }
        else
        {
            memcpy(pt + sizeof(SenderDataHeader), buffer, bytes);
        }
        pkts[slot].isRTX = false; pkts[slot].n_rtx = 0;
        ReleaseSemaphore(p->full, 1,NULL);
    }

    void WorkerRun()
    {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
        HANDLE events[] = { p->SocketRecieveReady,p->full };
        double timeout = 1;
        bool recompute = false;
        bool finAck = false;
        bool retrans = false;
        clock_t timerExpire = clock()+dynamicRTO.getRTO()*CLOCKS_PER_SEC;
        while (true)
        {
            if (!finAck)
            {
                //timeout = double(timerExpire - clock()) / CLOCKS_PER_SEC;
                //timeout = dynamicRTO.getRTO() * 1000;
                timeout = (double(timerExpire - clock()) / CLOCKS_PER_SEC)*1000;
                //printf("%f\n", timeout);
            }
            else
            {
                timeout = INFINITE;
            }
            recompute = false;
            //timeout = 2000;

            int ret = WaitForMultipleObjects(2, events, false, timeout);
            switch (ret)
            {
                case WAIT_TIMEOUT:
                {
                    //retransmission
                    sendto(sock, pkts[senderBase%W].getPkt(), pkts[senderBase % W].getSize(), 0, (struct sockaddr*)&remote, sizeof(remote));
                    p->timedOut += 1;
                    //dynamicRTO.doubleRTO();
                    pkts[senderBase % W].incrementRTX();
                    pkts[senderBase % W].setHasRTX();
                    pkts[senderBase % W].setTime();
                    retrans = true;

                    if (pkts[senderBase % W].getRTX() > 50)
                    {
                        printf("Error: max retransmission for packet %d reached (max=50))\n", senderBase);
                        finAck = true;
                    }
                    RTX = 0;
                    //printf("%f\n", dynamicRTO.getRTO());
                    recompute = true;
                }
                break;
                case WAIT_OBJECT_0+0: // events[0] aka socket recieve ready
                {
                    // recieve an ack // move senderBase; update RTT; handle fast retx; do flow control 
                    char* recBuf = new char[MAX_PKT_SIZE];
                    int r_len = sizeof(response);
                    int brec = recvfrom(sock, recBuf, MAX_PKT_SIZE, 0, (struct sockaddr*)&response, &(r_len));
                    if (brec == SOCKET_ERROR) { printf("ERROR deal with later"); }
                    ReceiverHeader* rh = (ReceiverHeader*)recBuf;
                    if ((rh->ackSeq > senderBase) && (rh->flags.ACK == 1))
                    {
                        int x = senderBase;
                        senderBase = rh->ackSeq;
                        p->senderBase = senderBase;
                        recompute = true;
                        //update RTT
                        if (!retrans/*!pkts[x%W].hasBeenRTX()*/)
                        {
                            double sampleRTT = double(clock() - pkts[(rh->ackSeq-1)%W].getTime()) / CLOCKS_PER_SEC;
                            //printf("%f\n", sampleRTT);
                            dynamicRTO.setSampleRTT(sampleRTT);
                            //printf("%f\n", dynamicRTO.getRTO());
                            p->estRTT = dynamicRTO.getEstimatedRTT();
                        }
                        if (senderBase >= lastBase)
                        {
                            finAck = true;
                            break;
                        }
                        
                        //rtx reset
                        RTX = 0;
                        retrans = false;
                        // flow control
                        int effectiveWin = min(W, rh->recvWnd);
                        // how much can we release semaphore
                        int newReleased = senderBase + effectiveWin - lastReleased;
                        ReleaseSemaphore(p->empty, newReleased, NULL);
                        lastReleased += newReleased;
                        

                    }
                    else
                    {
                        // deal with fast rtx
                        RTX += 1;

                        if (RTX == 3)
                        {
                            sendto(sock, pkts[senderBase%W].getPkt(), pkts[senderBase%W].getSize(), 0, (struct sockaddr*)&remote, sizeof(remote));
                            pkts[senderBase % W].setHasRTX();
                            pkts[senderBase % W].setTime();
                            //nextToSend = senderBase + 1;
                            p->next = nextToSend;
                            p->fastRet += 1;
                            RTX = 0;
                            retrans = true;
                            recompute = true;
                        }
                    }
                    delete[] recBuf;
                }   
                break;
                case WAIT_OBJECT_0+1:   // events[1] aka full
                {
                    //sender case
                    pkts[nextToSend%W].setTime(); // start timer
                    sendto(sock, pkts[nextToSend%W].getPkt(), pkts[nextToSend%W].getSize(), 0, (struct sockaddr*)&remote, sizeof(remote));
                    nextToSend++;
                    p->next = nextToSend;
                    //recompute = true;
                }
                break;
                case WAIT_FAILED:
                {
                    printf("%d", WSAGetLastError());

                }
                break;
            }
            if (recompute)
            {
                //recompute timerExpire
                timerExpire = clock() + dynamicRTO.getRTO() * CLOCKS_PER_SEC;

            }
            if (finAck)
            {
                SetEvent(p->eventQuit);
                break;
            }


        }
    }
    static void WINAPI StaticStart(void* param)
    {
        SenderSocket* This = (SenderSocket*)param;
        return This->WorkerRun();
    }
    void startWorker(HANDLE* handles)
    {
        handles[1]  = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StaticStart,(void*) this, 0, NULL);
    }
private:
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);
    SOCKET sock;
    double rto = 1;
    clock_t start_time;
    struct sockaddr_in remote;
    struct sockaddr_in response;
    bool connected = false;
    int seq = 0;
    RTT dynamicRTO;
    std::deque<Packet> pkts;
    int senderBase = 0;
    int nextToSend = 0;
    int W = 1;
    int lastReleased;
    int RTX;
    Parameters* p;
    UINT64 lastBase;
};


int main(int argc, char* argv[])
{
    if (argc != 8)
    {
        printf("Incorrect number of arguments, please provide the required 7 arguments\n");
        return 0;
    }
    char* targetHost = argv[1];
    int power = atoi(argv[2]);
    int senderWindow = atoi(argv[3]);
    double RTT = atof(argv[4]);
    double lossForward = atof(argv[5]);
    double lossReturn = atof(argv[6]);
    double bottleSpeed = atof(argv[7]); // Mbps

    printf("Main:\tsender W = %d, RTT %.3f sec, loss %g / %g, link %.2f Mbps\n",senderWindow,RTT,lossForward,lossReturn,bottleSpeed);
    printf("Main:\tinitializing DWORD array with 2^%d elements... done in ",power);
    // intitialize thread stuff here

    Parameters p;
    p.mutex = CreateMutex(NULL, 0, NULL);
    p.eventQuit = CreateEvent(NULL, true, false, NULL);
    p.SocketRecieveReady = CreateEvent(NULL, false, false, NULL);
    p.full = CreateSemaphore(NULL, 0, senderWindow, NULL);
    p.empty = CreateSemaphore(NULL, 0, senderWindow, NULL);
    p.winSize = senderWindow;

    // setup buffer
    clock_t tm = clock();
    UINT64 dwordBufSize = (UINT64) 1 << power;
    DWORD* dwordBuf = new DWORD[dwordBufSize]; // user-requested buffer 
    for (UINT64 i = 0; i < dwordBufSize; i++)
    {
        dwordBuf[i] = i;
    }
    tm = clock() - tm;
    double end_time = (double(tm) / CLOCKS_PER_SEC) * 1000;
    printf("%.2f ms\n", end_time);

    unsigned char* charBuf = (unsigned char*)dwordBuf;
    UINT64 byteBufferSize = dwordBufSize << 2;
    UINT64 off = 0;
    // successful binding
    // initialize sendersocket
    SenderSocket ss;
    tm = clock();
    LinkProperties lp;
    lp.RTT =RTT;
    lp.speed = 1e6 * bottleSpeed;
    lp.pLoss[FORWARD_PATH] = lossForward;
    lp.pLoss[RETURN_PATH] =  lossReturn;
    lp.bufferSize = senderWindow + 50;
    clock_t start = clock();
    clock_t transfer_time;
    int status = ss.Open(targetHost, MAGIC_PORT, senderWindow, &lp,&p,byteBufferSize);
    transfer_time = clock();
    double tm_elapsed = (double(clock() - start) / CLOCKS_PER_SEC);
    ss.getRTO().setEstimatedRTT(tm_elapsed);
    //p.estRTT = tm_elapsed;
    if (status != STATUS_OK)
    {
        printf("Main:\tconnect failed with status %d\n", status);
        return 0;
    }
    else
    {
        printf("Main:\tconnected to %s in %f sec, pkt size %d bytes\n", targetHost, p.estRTT, MAX_PKT_SIZE);
    }


    
    HANDLE* handles = new HANDLE[2];
    handles[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)statsThread, &p, 0, NULL);
    ss.startWorker(handles);// CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ss.WorkerRun(&p), &p, 0, NULL);

    // send here
    int timeouts =0;
    bool rtx = false;
    transfer_time = clock();
    int seq = 0;
    while (off < byteBufferSize)
    {
        int bytes = min(byteBufferSize - off, MAX_PKT_SIZE - sizeof(SenderDataHeader));
        int status;
        ss.Producer(charBuf+off,bytes,&p, seq);
        off += bytes;
        seq++;
    }
    //ss.Producer(charBuf, 0, &p, seq, true);
    
    double sendTime = double(clock() - transfer_time) / CLOCKS_PER_SEC;
    // signal stats thread to quit; wait for it to terminate
    //SetEvent(p.eventQuit);
    WaitForSingleObject(handles[0], INFINITE);
    WaitForSingleObject(handles[1], INFINITE);
    ss.Close(&p);
    //closed connection
    double transfer_elapsed = (double(clock() - transfer_time) / CLOCKS_PER_SEC);

    Checksum cs;
    DWORD check = cs.CRC32(charBuf, byteBufferSize);

    double rate = (double(dwordBufSize * 4 * 8) / sendTime) / 1000.0;
    printf("Main:\ttransfer finished in %.3f sec, %.3f Kbps, checksum %X\n", sendTime,rate,check);
    double ideal = (double((MAX_PKT_SIZE-sizeof(SenderDataHeader)) * 8)*senderWindow / ss.getRTO().getEstimatedRTT())/1000.0;
    printf("Main:\t estRTT %.3f, ideal rate %.2f Kbps\n",ss.getRTO().getEstimatedRTT(),ideal);
    
    return 0;
}

DWORD WINAPI statsThread(LPVOID pParam)
{
    Parameters* p = ((Parameters*)pParam);

    clock_t start = clock();
    int elapsedTime = 0;
    while (WaitForSingleObject(p->eventQuit, 2000) == WAIT_TIMEOUT)
    {
        elapsedTime += 2;
        double speed = double(8 * (MAX_PKT_SIZE - sizeof(SenderDataHeader)) * (p->senderBase - p->previousBase))/ 2000000.0;
        p->previousBase = p->senderBase;
        double data = double(p->senderBase * (MAX_PKT_SIZE - sizeof(SenderDataHeader))) / 1000000.0;
        //print stats
        printf("[ %d] B\t%d (%.2f MB) N\t %7d T %d F %d W %d S %.3f Mbps RTT %.3f\n", elapsedTime, p->senderBase,data, p->next, p->timedOut, p->fastRet, p->winSize,speed, p->estRTT);
    }
    double total_time = (double(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}

