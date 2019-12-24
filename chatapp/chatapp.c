#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>  
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define STDIN_BUF_SIZE 1024

void show_chat_menu ();
void start_chat ();
void view_your_message ();
void view_your_chat_history ();

void show_chat_menu (void)
{
	char input[STDIN_BUF_SIZE];
	int choice = 0;
	int stay = 0;
	do {
		memset (input, 0, STDIN_BUF_SIZE);
		fprintf (stdout, "\t\t1. Chat\n");
		fprintf (stdout, "\t\t2. View your messages\n");
		fprintf (stdout, "\t\t3. View your chat history\n");
		fprintf (stdout, "\t\t4. Exit\n");
		
		fprintf (stdout, "\t\tYour choice: ");
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

	switch (choice)
	{
	case 1: 
		start_chat ();
		break;
	case 2:	
		view_your_message ();		
		break;
	case 3: 
		view_your_chat_history();
		break;
	case 4: 
		exit (EXIT_SUCCESS);
	default:
		fprintf(stderr, "Invalid choice. Exiting...\n");
		exit (EXIT_FAILURE);
	}
}

void start_chat (char *my_username)
{
	int run = 1;
	char input[STDIN_BUF_SIZE];
	
	if (my_username == NULL)
	{
		fprintf (stderr, "Invalid user name. Exiting...\n");
		exit (EXIT_FAILURE);
	}

	memset (input, 0, STDIN_BUF_SIZE);
	fprintf (stdout, "Enter chat person name: ");
	fgets (input, STDIN_BUF_SIZE, stdin);
	fprintf (stdout, "Type your text: \n");
	
	do 
	{
		memset (input, 0, STDIN_BUF_SIZE);
		fprintf (stdout, "%s: ", my_username);
		fgets (input, STDIN_BUF_SIZE, stdin);
		if (strcmp (input, "bye") == 0)
			run  = 0;
	} while (run);
	
}

void view_your_message ()
{

}

void view_your_chat_history ()
{

}

int main (int argc, char *argv[])
{
	key_t key;
	int msgid;
	char buf[STDIN_BUF_SIZE];
	int choice = 0;
	memset (buf, 0, STDIN_BUF_SIZE);
	
	// 01. create token
	key = ftok ("/", 0x55);
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

	// 04. show chat menu
	show_chat_menu ();
	
	exit (EXIT_SUCCESS);
}

