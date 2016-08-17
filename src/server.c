# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <time.h>

# define SRV_PORT 6666
# define CLNT_PORT 4444
# define TALK_PORT 9997
# define MAX_SOCKET 5

int main ()
{
	short port;
	const int on = 1;
	fd_set rset, rset_child;
	int tcp_sock, ctcp_sock[MAX_SOCKET], from_len, max_sock, ready, 
	contact, c_contact[MAX_SOCKET], len, fd_fork, talk, i, count = 0;
	long int ttime;
	char buf[30];
	struct sockaddr_in s_addr, clnt_addr, new_s_addr[MAX_SOCKET], 
		talk_s_addr;
/*
 * Fabricate socket and set socket options.
 */
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	s_addr.sin_port = htons(SRV_PORT);

	talk_s_addr.sin_family = AF_INET; 
	talk_s_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	talk_s_addr.sin_port = htons(TALK_PORT);

	talk = socket (AF_INET, SOCK_DGRAM, 0); /* emergency socket */
	if(bind (talk, (struct sockaddr *)&talk_s_addr, 
		sizeof (talk_s_addr)) < 0)
	{
		perror ("Talk socket bind error!\n");
		exit (1);
	}
/*
 * TCP init part
 */
	tcp_sock = socket (AF_INET, SOCK_STREAM, 0);
	if(bind (tcp_sock, (struct sockaddr *)&s_addr, sizeof (s_addr)) < 0)
	{
		perror ("TCP bind error!\n");
		exit (1);
	}
	listen(tcp_sock, 1);

/*
 * Doughter part	
 */
	fd_fork = fork();
	if (fd_fork == (-1))
	{
		perror ("Fork create error!\n");
		exit(1);
	} 
	else if (fd_fork == 0)
	{
		close(tcp_sock);
		port = 5550;
		while (1)
		{
			len = sizeof (s_addr);
			from_len = recvfrom (talk, buf, 11, 0,
					(struct sockaddr *)&talk_s_addr, &len);
			if (from_len > 0)
			{
				port ++;
				write (1, buf, from_len);				
				new_s_addr[count].sin_family = AF_INET;
				new_s_addr[count].sin_addr.s_addr = 
					htonl(INADDR_LOOPBACK);
				new_s_addr[count].sin_port = htons(port);
				ctcp_sock[count] = 
					socket (AF_INET, SOCK_STREAM, 0);
				if (ctcp_sock[count] == -1)
				{
					perror ("Socket CTCP create error!\n");
					exit (1);
				}
				if (bind (ctcp_sock[count], 
					(struct sockaddr *)&new_s_addr[count],
					sizeof (new_s_addr[count])) < 0)
				{
					perror ("TCP child bind error!\n");
					exit (1);
				}
				listen (ctcp_sock[count], 1);
				c_contact[count] = 
					accept (ctcp_sock[count], 
					(struct sockaddr *)&new_s_addr[count],
					&len);
				if (c_contact[count] == (-1))
				{
					perror("Connect error!\n");
					exit (1);
				}
				from_len = recv (c_contact[count], buf, 21, 0);
				write (1, buf, from_len);
				ttime = time (NULL);
				send(c_contact[count], ctime (&ttime), 30, 0);
				close (c_contact[count]);
			}
			count++;
			if (count == MAX_SOCKET)
			{
				printf ("Socket limit is finished!\n");
				exit (1);
			}
		}
	close(talk);	
	close(ctcp_sock[count]);
	}
/*
 * Parent part.
 */
	port = 5550;	
	while (1)
	{
		port ++;
		len = sizeof (s_addr);
		contact = accept (tcp_sock, (struct sockaddr *)&s_addr, &len);
		sendto(talk, "INCOMING!\n", 11, 0,
				(struct sockaddr *)&talk_s_addr,
				sizeof(talk_s_addr));
		if (contact == (-1))
		{
			perror("Connect error!\n");
			exit(1);
		}
		from_len = recv (contact, buf, 27, 0);
		write (1, buf, from_len);
		send (contact, &port, sizeof(short), 0);
		close (contact);
	}
	port ++;
close(tcp_sock);
close(talk);							
return 1;
}
