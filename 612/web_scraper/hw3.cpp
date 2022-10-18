// Homework1_ReidOBoyle_612.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Reid OBoyle
// CSCE 612
// Spring 2022

#include "pch.h"
#include <iostream>
#pragma comment(lib, "ws2_32.lib")


class unique {
public:
	bool check_hosts(char* host)
	{
		int prevSize = seenHosts.size();
		seenHosts.insert(host);
		if (seenHosts.size() > prevSize)
		{
			// unique host
			return true;	
		}
		else {
			// duplicate host
			return false;
		}
	}
	bool checkIPs(DWORD IP)
	{
		int prevSize = seenIPs.size();
		seenIPs.insert(IP);
		if (seenIPs.size() > prevSize)
		{
			// unique IP
			return true;
		}
		else {
			// duplicate IP
			return false;
		}
	}
private:
	std::unordered_set<DWORD> seenIPs;
	std::unordered_set<std::string> seenHosts;
};

void cleanup(SOCKET* sock)
{
	closesocket(*sock);
	WSACleanup();
};

class Socket {
public:
	SOCKET* sock;
	char* buf;
	int allocatedSize;
	int curPos = 0;
	int initial_buf_size = 1000;
	struct fd_set fd;

	Socket(SOCKET *sk)
	{
		sock = sk;
		buf = (char *) malloc(initial_buf_size);
		allocatedSize = initial_buf_size;
	}
	bool Read(bool robot,bool multi=false)
	{
		int ret;
		struct timeval timeout;
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		clock_t start_time = clock();
		while (true)
		{
			FD_ZERO(&fd);
			FD_SET(*sock,&fd);

			//ret ;
			if ((ret = select(0, &fd, NULL, NULL, &timeout)) > 0)
			{
				clock_t total_time = (((double)clock() - start_time) / CLOCKS_PER_SEC) * 1000;
				//printf("%ld, ",total_time);
				if (total_time > 10000)
				{
					if (!multi) { printf("failed with long download\n"); }
					break;
				}
				int bytes = recv(*sock, buf + curPos, allocatedSize - curPos,0);

				if (bytes == SOCKET_ERROR)
				{
					if (!multi) { printf("failed with %d on recv\n", WSAGetLastError()); }
					break;
				}
				if (bytes == 0)
				{
					//NULL Terminate Buffer
					buf[curPos] = '\0';
					return true;
				}
				curPos += bytes;
				// 100 byte threshold
				if (allocatedSize - curPos < 100)
				{
					// resize buffer
					allocatedSize = allocatedSize * 2;
					buf = (char*)realloc(buf, allocatedSize);
				}
				if (allocatedSize > 2097152)
				{
					if (!multi) { printf("failed with exceeding 2MB max\n"); }
					break;
				}
				else if (robot and allocatedSize > 16000)
				{
					if (!multi) { printf("failed with exceeding 16KB max\n"); }
					break;
				}
			}
			else if (ret == 0)
			{
				if (!multi) { printf("failed with timeout\n"); }
				break;
			}
			else
			{
					if (!multi) { printf("failed with %d\n", WSAGetLastError()); }
				break;
			}
		}
		return false;
	}
	void free_buf()
	{
		free(buf);
	}
	void reset_buf()
	{
		buf = (char*)malloc(initial_buf_size);
		allocatedSize = initial_buf_size;
		curPos = 0;
	}
	void reset_sock(SOCKET* s)
	{
		sock = s;
	}
};

class Request {
public:
	Request(char* url)
	{
		//int len = strlen(url);
		char* r_end = strstr(url, "\r");
		if (r_end != nullptr)
		{
			*r_end = '\0';
		}
		char* hashtag = strrchr(url, '#');
		if (hashtag != NULL)
		{
			*hashtag = '\0';
		}
		char* qmark = strrchr(url, '?');
		if (qmark != NULL)
		{
			
			char* end = strrchr(url, '\0');
			request_len += (int)(end - qmark);
			
			query = qmark + 1;
			*qmark = '\0';
		}
		char scheme[8];
		strncpy_s(scheme, url, 7);
		scheme[7] = '\0';
		if (strcmp(scheme,"http://") != 0)
		{
			//printf("Error the scheme %s is not in the accepted format\n", scheme);
			invalid_scheme = true;
			return;
		}
		url = url + 7;
		char* slash = strchr(url, '/');
		if (slash != NULL)
		{
			char* end = strrchr(url, '\0');
			request_len += (int)(end - slash);

			path = slash + 1;
			*slash = '\0';
		}
		char* colon = strrchr(url, ':');
		if (colon != NULL)
		{
			port = colon + 1;
			*colon = '\0';
		}


		host = url;

	}

	char* get_host() { return host; }
	char* get_query() { return query; }
	char* get_path() { return path; }
	char* get_port() { return port; }
	bool get_inv() { return invalid_scheme; }
	int get_sz() { return request_len; }
	int get_port_int() { return port_int; }
	char* get_crawler() { return crawler; }
	char* get_request() { return request; }

	bool too_long_host()
	{
		char* end = strrchr(host, '\0');
		int host_len = end - host;
		if (host_len > MAX_HOST_LEN)
		{
			return true;
		}
		return false;
	}
	bool valid_port()
	{
		if (port == NULL) 
		{
			port_int = 80;
		}
		else
		{
			port_int = (int)atoi(port);
			if (port_int == 0)
			{
				return false;
			}
		}
		return true;
	}
	void build_request()
	{
		request = (char*)malloc(request_len + 2);
		if (path == NULL and query == NULL)
		{
			strcpy(request, "/");
		}
		else if (path == NULL)
		{
			sprintf(request, "/%s\0", query);
		}
		else if (query == NULL)
		{
			sprintf(request, "/%s\0", path);
		}
		else
		{
			int written = 0;
			written = sprintf(&request[written], "/%s", path);
			sprintf(&request[written], "?%s", query);
		}

	}
	void print_req(bool p2)
	{
		if (!p2)
		{
			printf("host %s, port %d, request %s\n", host, port_int, request);
		}
		else
		{
			printf("host %s, port %d\n", host, port_int);
		}
	}
	void release()
	{
		free(request);
	}

private:
	char* host = nullptr;
	char* query = nullptr;
	char* path = nullptr;
	char* port = nullptr;
	char* request = nullptr;
	char crawler[26] = "ReidOBoyleTAMUCrawler/1.3";
	int port_int;
	bool invalid_scheme = false;
	int request_len = 1;
};

int DNS(struct sockaddr_in* server, SOCKET* sock, Request* r,bool multi=false);
int initial_check(Request* r, bool p2,bool multi);
int connect_on_page(struct sockaddr_in* server, SOCKET* sock, Request* r, Socket* sk_handler, bool robot,bool multi);
int parse(Socket* sk_handler, Request* r, bool multi);
class QueueClass;


class Parameters {
public:
	HANDLE	mutex;
	LPCRITICAL_SECTION cs;
	HANDLE	finished;
	HANDLE	eventQuit;
	QueueClass* Q;			// Queue
	unique* uniq;
	int nThreads = 0;
	int E = 0;				// extracted URLs from queue
	int H = 0;				// number of URLS passed host uniqueness
	int D = 0;				// number of successful DNS lookups
	int I = 0;				// number of URLs passed IP uniqueness
	int R = 0;				// number of URLs that passed robots checks
	int C = 0;				// number of URLs crawled with valid HTTP code
	int L = 0;				// total links found
	double bytes = 0;
	double time = 0;
	int http_codes[6] = { 0,0,0,0,0,0 };
};


class QueueClass {
public:
	QueueClass()
	{

	}
	void Producer(int N, char** host)
	{
		// produce items into the queue
		for (int i = 0; i < N; i++)
		{
			Q.push(host[i]);
		}
	}
	void pushOne(char* host)
	{
		Q.push(host);
	}
	void Consumer(Parameters* p,HTMLParserBase* parser)
	{
		while (true)
		{
			WaitForSingleObject(p->mutex, INFINITE);
			//EnterCriticalSection(p->cs);
			if (Q.size() == 0)
			{
				ReleaseMutex(p->mutex);
				break;
			}
			char* x = Q.front();
			Q.pop();
			ReleaseMutex(p->mutex);	// LeaveCriticalSection(p->cs);
			p->E += 1;
			// crawl x
			SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			Request r = Request(x);
			r.build_request();
			
			char* tamu = strstr(r.get_host(), "tamu");

			
			if (!initial_check(&r, true,true))
			{
				free(x);
				continue;
			}
			WaitForSingleObject(p->mutex, INFINITE);
			if (p->uniq->check_hosts(r.get_host()) == false)
			{
				free(x);
				ReleaseMutex(p->mutex);
				continue;
			}	
			else { p->H += 1; }
			//ReleaseMutex(p->mutex);
			struct sockaddr_in server;
			int success = DNS(&server, &sock, &r,true);
			ReleaseMutex(p->mutex);
			if (!success)
			{
				free(x);
				continue;
			}
			else
			{
				// successful DNS
				p->D += 1;
				// checking IP Uniqueness
				DWORD ip = server.sin_addr.S_un.S_addr;
				WaitForSingleObject(p->mutex, INFINITE);
				if (p->uniq->checkIPs(ip) == false)
				{
					free(x);
					ReleaseMutex(p->mutex);
					continue;
				}
				ReleaseMutex(p->mutex);
				p->I += 1;
			}
			server.sin_family = AF_INET;		//IPv4
			server.sin_port = htons(r.get_port_int());

			Socket sk_handler(&sock);
			int status = connect_on_page(&server, &sock, &r, &sk_handler, true,true);
			p->bytes += sk_handler.curPos;
			if (status > 199 and status < 300) { p->http_codes[2] += 1; }
			else if (status >= 300 and status < 400) { p->http_codes[3] += 1; }
			else if (status >= 400 and status < 500) { p->http_codes[4] += 1; }
			else if (status >= 500 and status < 600) { p->http_codes[5] += 1; }
			else { p->http_codes[0] += 1; }

			if (status > 399 and status < 500)
			{
				p->R += 1;
				sk_handler.free_buf();
				sk_handler.reset_buf();
				closesocket(sock);
				sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				sk_handler.reset_sock(&sock);

				int status = connect_on_page(&server, &sock, &r, &sk_handler, false,true);
				p->bytes += sk_handler.curPos;
				if (status == 200)
				{

					p->C += 1;
					//int links = parse(&sk_handler, &r,false);
					
					char* header = sk_handler.buf;
					char* html_start = strstr(sk_handler.buf, "<html>");
					// parse here
					const int url_len = strlen(r.get_host()) + 7;
					char* baseURL = new char[url_len];
					sprintf(baseURL, "http://%s\0", r.get_host());
					int nLinks;

					char* linkBuffer = parser->Parse(sk_handler.buf, sk_handler.curPos, baseURL, (int)strlen(baseURL), &nLinks);

					if (nLinks > 0)
					{
						p->L += nLinks;
					}
				}
			}

			// free x
			closesocket(sock);
			free(x);
		}
	}
	std::queue<char*> getQ()
	{
		return Q;
	}
private:
	std::queue<char*> Q;
};




int initial_check(Request* r,bool p2,bool multi=false)
{
	if (!multi) { printf("\tParsing URL... "); }
	// Create request string
	if (r->get_inv())
	{
		if (!multi) { printf(" failed with invalid scheme\n"); }
		return 0;
	}
	if (r->get_sz() > MAX_REQUEST_LEN)
	{
		if (!multi) { printf(" failed with request longer than allowed size\n"); }
		return 0;
	}
	if(r->too_long_host())
	{
		if (!multi) { printf(" failed with host longer than allowed size\n"); }
		return 0;
	}


	if (!r->valid_port())
	{
		if (!multi) { printf(" failed with invalid port\n"); }
		return 0;
	}

	//printf("%s\n", request);
	if (!multi) { r->print_req(p2); }
	return 1;
};
int DNS(struct sockaddr_in* server,SOCKET* sock,Request* r,bool multi)
{
	// get host name
	char* host = r->get_host();

	if (!multi) { printf("\tDoing DNS... "); }
	clock_t tm = clock();
	DWORD ip_addr = inet_addr(host);
	if (ip_addr == INADDR_NONE)
	{
		struct hostent* remote;
		try {
			gethostbyname(host);
		}
		catch (...)
		{
			return 0;
		}
		remote = gethostbyname(host);
		if (remote == NULL)
		{
			if (!multi) { printf("failed with %d\n", WSAGetLastError()); }
			r->release();
			return 0;
		}
		else {
			memcpy((char*)&(server->sin_addr), remote->h_addr, remote->h_length);
		}
	}
	else
	{
		server->sin_addr.S_un.S_addr = ip_addr;
	}
	tm = (((double)clock() - tm) / CLOCKS_PER_SEC) * 1000;
	if (!multi) { printf("done in %d ms, found %s\n", tm, inet_ntoa(server->sin_addr)); }
	return 1;
};

void free_sk(Socket* sk_handler)
{
	sk_handler->free_buf();
};
int connect_on_page(struct sockaddr_in* server,SOCKET* sock,Request* r,Socket* sk_handler,bool robot,bool multi=false)
{
	
	clock_t tm = clock();
	if(robot and !multi) { printf("\tConnecting on robots... "); } else if (!multi) { printf("      * Connecting on page... "); }
	if (connect(*sock, (struct sockaddr*)&(*server), sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		if(!multi) {printf("failed with %d\n", WSAGetLastError());}
		return 0;
	}
	tm = (((double)clock() - tm) / CLOCKS_PER_SEC) * 1000;
	if (!multi) { printf(" done in %d ms\n", tm); }

	char* host = r->get_host();
	char* end = strrchr(host, '\0');
	int host_len = end - host;
	int r_len = r->get_sz();
	if (robot)
	{
		r_len = 11;
	}
	int buf_len = 51 + r_len + host_len + 26; // 51 for constant words and 26 for crawler agent name
	char* buf = new char[buf_len + 1];
	if (robot)
	{
		sprintf(buf, "HEAD %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n", "/robots.txt", r->get_host(), r->get_crawler());
	}
	else
	{
		sprintf(buf, "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n", r->get_request(), r->get_host(), r->get_crawler());
	}
	
	//printf("Request: %s\n", buf);
	// sending http requests
	tm = clock();

	int byteSent = send(*sock, buf, buf_len, 0);
	if (byteSent == SOCKET_ERROR)
	{
		if (!multi) { printf("failed with %d\n", WSAGetLastError()); }
		return 0;
	}
	//printf("\n%s\n", buf);

	
	if (!multi) { printf("\tLoading... "); }
	bool success = sk_handler->Read(robot,multi);

	if (!success)
	{
		return 0;
	}
	char* data = sk_handler->buf;
	char* header = data;
	char* html_start = strstr(data, "<html>");
	tm = (((double)clock() - tm) / CLOCKS_PER_SEC) * 1000;

	char http[6];
	strncpy(http, data, 5);
	http[5] = '\0';
	if (http[4] == 'S' || strcmp(http, "HTTP/\0") != 0)
	{
		if (!multi) { printf("failed with non-HTTP header\n"); }
		return 0;
	}

	if (!multi) { printf("done in %d ms with %d bytes\n", tm, sk_handler->curPos); }

	char status[4];
	strncpy(status, data + 9, 3);
	status[3] = '\0';
	if (!multi) { printf("\tVerifying Header... status code %s\n", status); }
	return atoi(status);

}
int parse(Socket* sk_handler, Request* r,bool multi=false)
{
	if (!multi) { printf("      + Parsing Page... "); }
	clock_t tm = clock();
	char* header = sk_handler->buf;
	char* html_start = strstr(sk_handler->buf, "<html>");
	// parse here
	HTMLParserBase* parser = new HTMLParserBase;
	const int url_len = strlen(r->get_host()) + 7;
	char* baseURL = new char[url_len];
	sprintf(baseURL, "http://%s\0", r->get_host());
	int nLinks;
	char* linkBuffer = parser->Parse(sk_handler->buf, sk_handler->curPos, baseURL, (int)strlen(baseURL), &nLinks);

	if (nLinks < 0)
	{
		nLinks = 0;
	}

	// record time
	tm = (((double)clock() - tm) / CLOCKS_PER_SEC) * 1000;
	if (!multi) { printf("done in %d ms with %d links\n\n", tm, nLinks); }
	return nLinks;
}

DWORD WINAPI statsThread(LPVOID pParam)
{
	Parameters* p = ((Parameters*)pParam);
	clock_t start_time = clock();
	int elapsedTime = 0;
	int previousPages = 0;
	//printf("[%3d] %3d Q %6d E %7d H %6d D %6d I %5d R %5d C %5d L %4d \n", elapsedTime, p->nThreads, p->Q->getQ().size(), p->E, p->H, p->D, p->I, p->R, p->C, p->L);
	double previousBytes = 0;
	while (WaitForSingleObject(p->eventQuit,2000) == WAIT_TIMEOUT)
	{
		elapsedTime += 2;
		double numBytes = p->bytes - previousBytes;
		double numPages = p->C - previousPages;
		printf("[%3d] %4d Q %6d E %7d H %6d D %6d I %5d R %5d C %5d L %4g K\n", elapsedTime,p->nThreads, p->Q->getQ().size(), p->E, p->H, p->D, p->I, p->R, p->C, floor(double(p->L)/1000.00));
		printf("\t*** crawling %f pps @ %f Mbps\n", numPages/2.0, numBytes/double(2000.0));
		previousBytes = p->bytes;
		previousPages = p->C;
	}
	clock_t tm = (((double)clock() - start_time) / CLOCKS_PER_SEC) ;
	p->time = tm;

	//ReleaseSemaphore(p->finished, 1, NULL);
	return 0;
}
DWORD WINAPI crawlerThread(LPVOID pParam)
{
	Parameters* p = (Parameters*)pParam;
	HTMLParserBase* parser = new HTMLParserBase;
	/*
	while (true)
	{
		WaitForSingleObject(p->mutex, INFINITE);
		//EnterCriticalSection(p->cs);
		if (p->Q->getQ().size() == 0)
		{
			break;
		}
		//LeaveCriticalSection(p->cs);
		ReleaseMutex(p->mutex);
		p->Q->Consumer(p, parser);
	}*/
	p->Q->Consumer(p, parser);
	//ReleaseSemaphore(p->finished, 1,NULL);
	p->nThreads -= 1;
	delete parser;
	return 0;
}
int _tmain(int argc, char* argv[])
{
	// parse command line args
	int threads = atoi(argv[1]);
	char* filename = argv[2];

	clock_t elapsedTime;	// time taken
	int Q = threads;		// current size of pending queue



	// initialize shared data structures & parameters sent to threads
	QueueClass q = QueueClass();
	Parameters p;
	unique u = unique();
	p.mutex = CreateMutex(NULL, 0, NULL);						// create a mutex for accessing critical sections (including printf); initial state = not locked
	p.finished = CreateSemaphore(NULL, 0, threads, NULL);		// create a semaphore that counts the number of active threads; initial value = 0, max = threads
	p.eventQuit = CreateEvent(NULL, true, false, NULL);			// create a quit event; manual reset, initial state = not signaled
	p.Q = &q;
	p.uniq = &u;
	

	// read file and populate shared queue
	HANDLE hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("File error opening: %s with %d\n", filename, GetLastError());
		return 0;
	}
	LARGE_INTEGER li;
	bool bRet = GetFileSizeEx(hFile, &li);
	// file size error
	if (bRet == 0)
	{
		printf("File Size error\n");
		CloseHandle(hFile);
		return 0;
	}

	int fileSize = (DWORD)li.QuadPart;
	DWORD bytesRead;
	// allocate buffer;
	char* fileBuf = new char[fileSize];
	char* movingBuf = fileBuf;
	bRet = ReadFile(hFile, fileBuf, fileSize, &bytesRead, NULL);
	// process errors
	if (bRet == 0 || bytesRead != fileSize)
	{
		printf("Read file failed with %d\n", GetLastError());
		delete fileBuf;
		CloseHandle(hFile);
		return 0;
	}
	printf("Opened %s with size %d\n", filename, bytesRead);
	//printf("%s\n", fileBuf);
	char* end = strchr(fileBuf, '\0');
	int N = 0;
	
	while (movingBuf < end)
	{
		char* nl = strchr(movingBuf, '\n');
		if (nl == nullptr or *movingBuf == '\0')
		{
			break;
		}
		else
		{
			N += 1;
			int len = nl - movingBuf;
			char* url = (char*)malloc(len + 1);
			strncpy_s(url, len + 1, movingBuf, len);
			url[len] = '\0';
			q.pushOne(url);

			//free(url);
			movingBuf = nl + 1;
		}
	}
	delete fileBuf;
	CloseHandle(hFile);

	//InitializeCriticalSection(p.cs);
	HANDLE* handles = new HANDLE[threads+4];
	// start stats thread
	handles[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)statsThread, &p, 0, NULL);
	// start N crawling threads
	for (int i = 1; i < threads+1; i++)
	{
		handles[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)crawlerThread, &p, 0, NULL);
		p.nThreads += 1;
	}
	// wait for N crawling threads to finish
	for (int i = 1; i < threads+1; i++)
	{
		WaitForSingleObject(handles[i], INFINITE);
		CloseHandle(handles[i]);
	}
	// signal stats thread to quit; wait for it to terminate
	SetEvent(p.eventQuit);
	WaitForSingleObject(handles[0], INFINITE);

	double url_rate = double(p.E)/p.time;
	double look_up_rate = double(p.H)/p.time;
	double robots_rate = double(p.I)/p.time;
	double crawl_rate= double(p.C)/p.time;
	double parse_rate= double(p.L)/p.time;
	double megabytes = round(p.bytes/1000000);

	//printf("%f\n", p.time);

	printf("Extracted %d URLs @ %f/s\nLooked Up %d DNS names @ %f/s\nAttempted %d robots @ %f/s\nCrawled %d pages @ %f/s (%f MB)\nParsed %d links @ %f/s\nHTTP codes: 2xx = %d, 3xx = %d, 4xx = %d, 5xx = %d, other = %d\n",p.E,url_rate,p.H,look_up_rate,p.I,robots_rate,p.C,crawl_rate,megabytes,p.L,parse_rate,p.http_codes[2],p.http_codes[3],p.http_codes[4],p.http_codes[5],p.http_codes[0]);
	// cleanup

	return 0;
}

int main(int argc,char *argv[])
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("WSAStartup Error %d\n", WSAGetLastError());
		WSACleanup();
		exit(-1);
	}
	
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		printf("socket() error %d\n", WSAGetLastError());
		cleanup(&sock);
		exit(-1);
	}
	/* Part 2 Implemenation*/
	if (argc == 3)
	{
		
		int threads = atoi(argv[1]);
		
		if (threads != 1)
		{
			//part3
			
			return _tmain(argc, argv);

			//printf("You indicated a number of threads other than 1, please only use 1 thread.\n");
			cleanup(&sock);
			return 0;
		}
		char* filename = argv[2];
		HANDLE hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			printf("File error opening: %s with %d\n",filename,GetLastError());
			cleanup(&sock);
			return 0;
		}
		LARGE_INTEGER li;
		bool bRet = GetFileSizeEx(hFile, &li);
		// file size error
		if (bRet == 0)
		{
			printf("File Size error\n");
			CloseHandle(hFile);
			cleanup(&sock);
			return 0;
		}

		int fileSize = (DWORD)li.QuadPart;
		DWORD bytesRead;
		// allocate buffer;
		char* fileBuf = new char[fileSize];
		char* movingBuf = fileBuf;
		bRet = ReadFile(hFile, fileBuf, fileSize, &bytesRead, NULL);
		// process errors
		if (bRet == 0 || bytesRead != fileSize)
		{
			printf("Read file failed with %d\n", GetLastError());
			delete fileBuf;
			cleanup(&sock);
			CloseHandle(hFile);
			return 0;
		}
		printf("Opened %s with size %d\n", filename, bytesRead);
		//printf("%s\n", fileBuf);
		char* end = strchr(fileBuf, '\0');
		unique u = unique();
		while (movingBuf < end)
		{
			char* nl = strchr(movingBuf,'\n');
			if (nl == nullptr or *movingBuf == '\0')
			{
				break;
			}
			else
			{
				int len = nl - movingBuf;
				char* url = (char*)malloc(len +1);
				strncpy_s(url,len+1,movingBuf,len);
				url[len] = '\0';
				//printf("%s\n", url);
				printf("URL: %s\n", url);
				// now do part1 stuff
				Request r = Request(url);
				r.build_request();
				if (!initial_check(&r,true,false))
				{
					free(url);
					movingBuf = nl + 1;
					continue;
				}
				printf("\tChecking host uniqueness... ");
				if (u.check_hosts(r.get_host()) == false)
				{
					printf("failed\n");
					free(url);
					movingBuf = nl + 1;
					continue;
				}
				else
				{
					printf("passed\n");
				}
				struct sockaddr_in server;
				int success = DNS(&server, &sock, &r,false);
				if (!success)
				{
					free(url);
					movingBuf = nl + 1;
					continue;
				}
				else
				{
					printf("\tChecking IP Uniqueness... ");
					DWORD ip = server.sin_addr.S_un.S_addr;
					if (u.checkIPs(ip) == false)
					{
						printf("failed\n");
						free(url);
						movingBuf = nl + 1;
						continue;
					}
					else
					{
						printf("passed\n");
					}
				}
				server.sin_family = AF_INET;		//IPv4
				server.sin_port = htons(r.get_port_int());

				Socket sk_handler(&sock);
				int status = connect_on_page(&server, &sock, &r, &sk_handler, true);
				if(status > 399 and status < 500)
				{
				
					sk_handler.free_buf();
					sk_handler.reset_buf();
					closesocket(sock);
					WSACleanup();
					WSAStartup(wVersionRequested, &wsaData);
					sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					sk_handler.reset_sock(&sock);
					int status = connect_on_page(&server, &sock, &r, &sk_handler, false);
					if (status == 200)
					{
						parse(&sk_handler, &r);
					}
				}
				sk_handler.free_buf();
				sk_handler.reset_buf();
				closesocket(sock);
				WSACleanup();
				WSAStartup(wVersionRequested, &wsaData);
				sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				sk_handler.reset_sock(&sock);

				free(url);
				movingBuf = nl+1;
			}


		}

		//close file
		CloseHandle(hFile);
		// cleanup
		free(fileBuf);

	}
	else if (argc == 2)
	{
		printf("URL: %s\n", argv[1]);
		Request r = Request(argv[1]);
		r.build_request();
		if (!initial_check(&r,false,false))
		{
			return 0;
		}
		
		struct sockaddr_in server;
		int success = DNS(&server,&sock, &r,false);
		// get host name, set port to -1 on failed DNS search
		if (!success)
		{
			cleanup(&sock);
			return 0;
		}
		server.sin_family = AF_INET;		//IPv4
		server.sin_port = htons(r.get_port_int());

		Socket sk_handler(&sock);
		int status = connect_on_page(&server, &sock, &r,&sk_handler, false);
		if ( status == 0)
		{
			return 0;
		}
		// successful connection
		if (status != 200)
		{
			char* html_start = strstr(sk_handler.buf, "<html>");
			if (html_start != nullptr)
			{
				*html_start = '\0';
			}
			printf("\n----------------------------------------------\n%s", sk_handler.buf);

		}
		else
		{
			printf("      + Parsing Page... ");
			clock_t tm = clock();
			char* header = sk_handler.buf;
			char* html_start = strstr(sk_handler.buf, "<html>");
			// parse here
			HTMLParserBase* parser = new HTMLParserBase;
			const int url_len = strlen(r.get_host()) + 7;
			char* baseURL = new char[url_len];
			sprintf(baseURL, "http://%s\0", r.get_host());
			int nLinks;
			char* linkBuffer = parser->Parse(sk_handler.buf, sk_handler.curPos, baseURL, (int)strlen(baseURL), &nLinks);

			if (nLinks < 0)
			{
				nLinks = 0;
			}

			// record time
			tm = (((double)clock() - tm) / CLOCKS_PER_SEC) * 1000;
			printf("done in %d ms with %d links\n\n", tm, nLinks);
			if (html_start != nullptr)
			{
				*html_start = '\0';
			}
			printf("----------------------------------------------\n%s", sk_handler.buf);
			sk_handler.free_buf();

		}
		r.release();
	}
	else
	{
		printf("Invalid arguments, please only supply one URL\n");
		cleanup(&sock);
		return 0;
	}

	//connect socket to server


	//printf("Successfully connected to %s (%s) on port %d\n", r.get_host(), inet_ntoa(server.sin_addr), htons(server.sin_port));
	
	// ending cleanup
	closesocket(sock);
	WSACleanup();
	return 0;
}
