#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>  
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define STDIN_BUF_SIZE 1024
#define USERNAME_SIZE  100

int show_chat_menu ();
void start_chat (char *my_username, int msgid);
void view_your_message (char *my_username, int msgid);
void view_your_chat_history ();

int g_running = 1;
typedef struct {
	char message [STDIN_BUF_SIZE];
	char from [USERNAME_SIZE];	//username
	char to [USERNAME_SIZE];	//username
	//int len;
} ChatMsg;

struct msgbuf {
	long mtype;
	ChatMsg mtext;
};

long get_hash (char *str)
{
	int i;
	int ascii;
	char c;
	long hash=0;
	if (str == NULL)
		return 0;
	if (strlen (str) == 0)
		return 0;

	for (i=0; i<strlen(str); i++)
	{
		c = str [i];
		ascii = c - '0';
		hash = (hash * 10) + ascii;
	}

	return hash;
}

void remove_newline_char_and_copy (char *dest, const char *src, int dest_size)
{
	int i;
	if (strlen (src) > dest_size)
	{
		fprintf (stderr, "ERROR: len of src > dest. Exiting...\n");
		exit (EXIT_FAILURE);
	}
	for (i=0; i<strlen(src); i++)
	{
		if (src[i] != '\n')
		{
			dest [i] = src [i];
		}
	}
}

int show_chat_menu (void)
{
	char input[STDIN_BUF_SIZE];
	int choice = 0;
	int stay = 0;
	do {
		memset (input, 0, STDIN_BUF_SIZE);
		fprintf (stdout, "==============================================\n");
		fprintf (stdout, "\t\t1. Chat\n");
		fprintf (stdout, "\t\t2. View your messages\n");
		fprintf (stdout, "\t\t3. View your chat history\n");
		fprintf (stdout, "\t\t4. Exit\n");
		
		fprintf (stdout, "\tYour choice: ");
		fgets (input, STDIN_BUF_SIZE, stdin);
		choice = atoi (input);
		if (choice < 1 || choice > 4)
		{
			fprintf (stdout, "\t\tInvalid choice (%d). Provide again.\n", choice);
			stay = 1;
		}
		else
		{
			stay = 0;
		}
	} while (stay);

	fprintf (stdout, "==============================================\n");
	return choice;
}

void start_chat (char *my_username, int msgid)
{
	int run = 1, ret = 0;
	char input[STDIN_BUF_SIZE];
	struct msgbuf msgbuf;
	memset (&msgbuf, 0, sizeof (struct msgbuf));
	
	if (my_username == NULL)
	{
		fprintf (stderr, "Invalid user name. Exiting...\n");
		exit (EXIT_FAILURE);
	}

	strncpy (msgbuf.mtext.from, my_username, USERNAME_SIZE);

	// get username to chat with
	memset (input, 0, STDIN_BUF_SIZE);
	fprintf (stdout, "Enter chat person name: ");
	fgets (input, STDIN_BUF_SIZE, stdin);
	remove_newline_char_and_copy (msgbuf.mtext.to, input, USERNAME_SIZE);
	msgbuf.mtype = get_hash (msgbuf.mtext.to);

	fprintf (stdout, "Type your text: \n");

	do 
	{
		// get message from stdin
		memset (input, 0, STDIN_BUF_SIZE);
		fprintf (stdout, "%s: ", my_username);
		fgets (input, STDIN_BUF_SIZE, stdin);

		// send message to message queue
		memset (msgbuf.mtext.message, 0, STDIN_BUF_SIZE);
		remove_newline_char_and_copy (msgbuf.mtext.message, input, STDIN_BUF_SIZE);
		//msgbuf.mtype = msgbuf.mtype + 1;
		//msgbuf.mtype = 10;
		ret = msgsnd (msgid, &msgbuf, sizeof(ChatMsg), 0);
		if (ret < 0)
		{
			fprintf (stderr, "ERROR: msgsnd: %s\n", strerror (errno));
		}

		if (strncmp (input, "bye", 3) == 0)
			run  = 0;
	} while (run);
	
}

void view_your_message (char *my_username, int msgid)
{
	int flag_fetch_more;
	ssize_t msg_size = 0;
	long hash = get_hash (my_username);
	struct msgbuf msgbuf;
	memset (&msgbuf, 0, sizeof (msgbuf));

	flag_fetch_more = 1;
	do
	{
		msg_size = msgrcv (msgid, &msgbuf, sizeof (ChatMsg), hash, IPC_NOWAIT);
		if (msg_size < 0)
		{
			//fprintf (stdout, "No message for you\n");
			flag_fetch_more = 0;
		}
		else
		{
			fprintf (stdout, "%s says: %s\n", msgbuf.mtext.from, msgbuf.mtext.message);
		}
	} while (flag_fetch_more);

}

void view_your_chat_history ()
{

}

int main (int argc, char *argv[])
{
	key_t key;
	int msgid, i;
	char buf[USERNAME_SIZE];
	char *my_username = NULL;
	int choice = 0;
	memset (buf, 0, STDIN_BUF_SIZE);
	
	// 01. create token
	key = ftok ("/", 0x11);
	if (key < 0)
	{
		fprintf (stderr, "ERROR: ftok: %s\n", strerror (errno));
		exit (EXIT_FAILURE);
	}

	// 02. get message queue
	msgid = msgget (key, 0666 | IPC_CREAT); // Read write
	if (msgid < 0)
	{
		fprintf (stderr, "ERROR: msgget: %s\n", strerror (errno));
		exit (EXIT_FAILURE);
	}

	// 03. show some messages and prompt for user name
	fprintf (stdout, "Welcome!\n");
	fprintf (stdout, "\tEnter your chat alias name: ");
	fgets (buf, STDIN_BUF_SIZE, stdin);
	my_username = malloc (sizeof(buf));
	remove_newline_char_and_copy (my_username, buf, USERNAME_SIZE);

	if (strlen (my_username)<=0)
	{
		fprintf (stderr, "ERROR:Invalid aliase name. Exiting...\n");
		exit (EXIT_FAILURE);
	}

	// 04. show chat menu
	g_running = 1;
	do
	{
		choice = show_chat_menu ();

		switch (choice)
		{
		case 1:
			start_chat (my_username, msgid);
			break;
		case 2:
			view_your_message (my_username, msgid);
			break;
		case 3:
			view_your_chat_history();
			break;
		case 4:
			g_running = 0;
			break;
		default:
			fprintf(stderr, "Invalid choice. Exiting...\n");
			exit (EXIT_FAILURE);
		}
	}while (g_running);

	// release resource
	if (my_username)
	{
		free (my_username);
	}
	exit (EXIT_SUCCESS);
}

