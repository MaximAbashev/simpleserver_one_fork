# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <time.h>

# define SRV_PORT 6666
# define CLNT_PORT 4444

int main ()
{
	short port;
	int sock, new_sock;
	int from_len, len;
	char buf[30];
	struct sockaddr_in clnt_addr, s_addr, new_s_addr;
/*
 * Initiate socket and fabricate socket options.
 */
	sock = socket (AF_INET, SOCK_STREAM, 0);
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);
	s_addr.sin_port = htons (SRV_PORT);
/*
 * Connecting to first socket and sending message.
 */	
	printf ("Connecting to first socket...\n");
	connect (sock, (struct sockaddr *)&s_addr, sizeof (s_addr));
	send (sock, "Can you give me new port?\n", 27, 0);
	from_len = recv (sock, &port, sizeof (short), 0);
	printf ("I take port: %d\n", port);
	close (sock);
	sleep (5);
/*
 * Connecting to second socket and sending message.
 */
	new_sock = socket (AF_INET, SOCK_STREAM, 0);
	new_s_addr.sin_family = AF_INET;
	new_s_addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);
	new_s_addr.sin_port = htons (port);
	printf ("Connecting to second socket...\n");
	connect (new_sock, (struct sockaddr *)&new_s_addr, sizeof (new_s_addr));
	send (new_sock, "How much o'clock?\n", 19, 0);
	sleep (20);
	from_len = recv (new_sock, buf, 30, 0);
	write (1, buf, from_len);
	close (new_sock);
	return 1;
}
