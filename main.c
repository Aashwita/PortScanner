#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT_TIMEOUT 1000

const char* get_service(int port)
{
    switch (port)
    {
        case 20:
        case 21: return "FTP";
        case 22: return "SSH";
        case 23: return "Telnet";
        case 25: return "SMTP";
        case 53: return "DNS";
        case 80: return "HTTP";
        case 110: return "POP3";
        case 143: return "IMAP";
        case 443: return "HTTPS";
        case 3306: return "MySQL";
        case 3389: return "RDP";
        default: return "Unknown";
    }
}

int scan_port(const char *ip, int port)
{
    SOCKET sock;
    struct sockaddr_in server;
    int result;
    DWORD timeout = PORT_TIMEOUT;

    /* Create TCP socket */
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock == INVALID_SOCKET)
    {
        return 0;
    }

    /* Set timeout */
    setsockopt(
        sock,
        SOL_SOCKET,
        SO_RCVTIMEO,
        (const char *)&timeout,
        sizeof(timeout)
    );

    setsockopt(
        sock,
        SOL_SOCKET,
        SO_SNDTIMEO,
        (const char *)&timeout,
        sizeof(timeout)
    );

    /* Configure target */
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server.sin_addr) != 1)
    {
        closesocket(sock);
        return 0;
    }

    /* Try connecting */
    result = connect(
        sock,
        (struct sockaddr *)&server,
        sizeof(server)
    );

    closesocket(sock);

    if (result == 0)
        return 1;

    return 0;
}

int main()
{
    WSADATA wsaData;

    char ip[100];
    int start_port;
    int end_port;
    int port;
    int open_ports = 0;

    /* Initialize Winsock */
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("ERROR: Winsock initialization failed.\n");
        return 1;
    }

    printf("=============================================\n");
    printf("       NETWORK SERVICE / PORT SCANNER\n");
    printf("=============================================\n\n");

    /* Input */
    printf("Enter Target IP Address: ");
    scanf("%99s", ip);

    printf("Enter Starting Port: ");
    scanf("%d", &start_port);

    printf("Enter Ending Port: ");
    scanf("%d", &end_port);

    /* Validate port range */
    if (start_port < 1 ||
        end_port > 65535 ||
        start_port > end_port)
    {
        printf("\nERROR: Invalid port range.\n");
        printf("Port range must be between 1 and 65535.\n");

        WSACleanup();
        return 1;
    }

    printf("\n=============================================\n");
    printf("Target IP    : %s\n", ip);
    printf("Port Range   : %d - %d\n",
           start_port, end_port);
    printf("=============================================\n");

    printf("\nScanning...\n\n");

    /* Scan each port */
    for (port = start_port; port <= end_port; port++)
    {
        if (scan_port(ip, port))
        {
            printf(
                "Port %5d : OPEN    Service: %s\n",
                port,
                get_service(port)
            );

            open_ports++;
        }
    }

    printf("\n=============================================\n");
    printf("Scan Completed!\n");
    printf("Total Open Ports: %d\n", open_ports);
    printf("=============================================\n");

    /* Cleanup Winsock */
    WSACleanup();

    return 0;
}
