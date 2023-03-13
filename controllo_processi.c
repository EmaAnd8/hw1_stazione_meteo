/*Una azienda agricola vuole gestire una stazione di monitoraggio per una serra domotica  con tre componenti termostato, sensore di umidità e sensore di luminosità ,controllati da tre processi  imparentati
P1,P2,P3.

		-P1 estrae un valore randomico tra 0 e 50 gradi e lo manda alla stazione meteo processo padre
		-P2 estrae un valore randomico tra 0 e 100% e lo manda alla stazione meteo processo padre 
		-P3 estrae un valore randomico tra 0 e 300 e lo manda alla stazione meteo processo padre

il valore viene mandato ogni 2 secondi  al processo padre 

il padre se uno dei tre processi supera il valore di riferimento

		-P1 se supera il valore di 13 gradi raffredda l'ambiente e abbassa la temperatura di 2 gradi
		-P2 se supera il valore di 50 abbassa l'umidità relativa di 5
		-P3 se supera il valore 290 abbassa la luminosità di 100

dopo 60 misurazioni la stazione meteo si spegne :
			
		-attende la terminazione dei figli
		-stampa a video gli utimi valori ricevuti 
		-cancella la coda di messaggi 
		-termina essa stessa

adottare il meccanismo delle code di messaggi SYSTEM 5 per la comunicazione interprocesso 



*/


#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/msg.h>
#include<unistd.h>
#include<time.h>

#define MSG_KEY (key_t)9873


//struct per definire il tipo di dato
struct dati
{
     
       int value;      

}valore;

struct msgbuf {

	long mtype;
	struct dati val;   //struct necessaria per il funzionamento delle code di messaggi

};


void msg_destroy(int msgid);
void child_function();
void child_function2();
void child_function3();

int main(void)
{

	//dichiarazione variabili di interesse

	int msgid;
	int i;
	int check;
	pid_t pid[3];
	struct msgbuf msg;



	//creazione della coda di messaggi
	msgid=msgget(MSG_KEY, 0666 | IPC_CREAT);
	if(msgid==-1)
	{
		perror("queque creation failed");
		exit(EXIT_FAILURE);
	}


	


	//creazione dei processi figli
        for(i=0;i<3;i++)
	{
		pid[i]=fork();

		switch(pid[i]){

			case -1:
			perror("process creation failed");
			exit(EXIT_FAILURE); 
			break;

		        case 0:
			if(i==0)
			child_function();
			if(i==1)
			child_function2();
			if(i==2)
			child_function3();

			exit(EXIT_SUCCESS); // eseguo solo il codice del figlio

			default:
			break;




		
		}

	}




	//gestione della ricezione dei messaggi
	for(i=0;i<60;i++)
	{
		
		check=msgrcv(msgid,(void*)&msg,sizeof(struct dati),0,0);
		if(check==-1)
		{
			perror("queque message ext failed");
			msg_destroy(msgid);
			exit(EXIT_FAILURE);

		}

		switch(msg.mtype)
		{
			case 1:
				if(msg.val.value>13)
				{
					msg.val.value=msg.val.value-2;
					msgsnd(msgid,(void*)&msg,sizeof(struct dati) ,0);
				}
			break;

			case 2:
				if(msg.val.value>50)
				{
					msg.val.value=msg.val.value-5;
					msgsnd(msgid,(void*)&msg,sizeof(struct dati) ,0);
				}
			break;

			case 3: 
				if(msg.val.value>290)
				{
					msg.val.value=msg.val.value-100;
					msgsnd(msgid,(void*)&msg,sizeof(struct dati) ,0);
				}
			break;

			default:
			break;
		}

		printf("l'ultimo valore ricevuto di i: \t  %d \n",i);
		sleep(2);
		
	}


	

	

	
	wait(NULL);
	wait(NULL);
	wait(NULL);
	
        //stampa ultimo valore ricevuto
	printf("l'ultimo valore ricevuto di %ld: \t  %d \n",msg.mtype,msg.val.value);



	//distruzione della coda di messaggi
	msg_destroy(msgid);


	//terminazione del padre 
	exit(EXIT_SUCCESS);


}



void child_function()
{
	int res;
	int j;
	int msgid;
 	struct msgbuf message;

	// attacchiamo il figlio alla coda
	msgid=msgget(MSG_KEY,0666);
	if(msgid==-1)
	{
		perror("queque creation failed");
		exit(EXIT_FAILURE);
	}


	
	//genero un valore casuale
	srand(time(NULL));

	//gestione del funzionamento dei processi figli

	for(j=0; j<60;j++)
	{
		int tmp=1+rand()%50;
		message.mtype=1;
		message.val.value=tmp;

		printf("I'm the temperature sensor with PID %d and I've found this temperature value: %d°C \n",getpid(),tmp);

		res=msgsnd(msgid,(void*)&message,sizeof(struct dati),0);
		if(res==-1)
		{
			perror("An error occurred while sending the message");
			exit(EXIT_FAILURE);
		}


		if(tmp>13)
		{
			res=msgrcv(msgid,(void*)&message,sizeof(struct dati),0,0);
			if(res==-1)
			{
				perror("queque message ext failed");
				exit(EXIT_FAILURE);

			}
		}
		

		sleep(2);

	}



	//terminazione del processo
	exit(EXIT_SUCCESS);

}


void child_function2()
{

	int res;
	int j;
	int msgid;
 	struct msgbuf message;

	// attacchiamo il figlio alla coda
	msgid=msgget(MSG_KEY,0666);
	if(msgid==-1)
	{
		perror("queque creation failed");
		exit(EXIT_FAILURE);
	}


	

	//genero un valore casuale
	srand(time(NULL));

	//gestione del funzionamento dei processi figli

	for(j=0; j<60;j++)
	{
		int hum=1+rand()%100;
		message.mtype=2;
		message.val.value=hum;

		printf("I'm the humidity sensor with PID %d and I've found this relative humidity value: \t %d \n",getpid(),hum);

		res=msgsnd(msgid,(void*)&message,sizeof(struct dati),0);
		if(res==-1)
		{
			perror("An error occurred while sending the message");
			exit(EXIT_FAILURE);
		}


		if(hum>50)
		{
			res=msgrcv(msgid,(void*)&message,sizeof(struct dati),0,0);
			if(res==-1)
			{
				perror("queque message ext failed");
				exit(EXIT_FAILURE);

			}
		}
		

		sleep(2);

	}



	//terminazione del processo
	exit(EXIT_SUCCESS);




}


void child_function3()
{

	int j;
	int msgid;
	int res;
	struct msgbuf message;

	//inserimento del processo figlio nella coda 
	msgid=msgget(MSG_KEY,0666);
	if(msgid==-1)
	{
		perror("queque creation failed");
		exit(EXIT_FAILURE);
	}

	srand(time(NULL));
	
	for(j=0;j<60;j++)
	{

		int lum=1+rand()%300;
		message.mtype=3;
		message.val.value=lum;
		
		printf("I'm the light sensor with PID %d and I've found this light value: %d\n",getpid(),lum);
		res=msgsnd(msgid,(void*)&message,sizeof(struct dati),0);
		if(res==-1)
		{
			perror("An error occurred while sending the message");
			exit(EXIT_FAILURE);
		}

		if(lum>290)
		{
			res=msgrcv(msgid,(void*)&message,sizeof(struct dati),0,0);
			if(res==-1)
			{
				perror("queque message ext failed");
				exit(EXIT_FAILURE);

			}
		}
		

		sleep(2);
	}


	//terminazione dei processo
	exit(EXIT_SUCCESS);



}


void msg_destroy(int msgid)
{

	int res;
	
	res=msgctl(msgid,IPC_RMID,NULL);
	if(res==-1)
	{

		perror("The queue cancellation doesn't work");
		exit(EXIT_FAILURE);
		
	}


}






