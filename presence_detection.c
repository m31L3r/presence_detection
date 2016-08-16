// Präsenz-Detektion mit Omron D6T-44L-06
// Anschließende Ausgabe einer Regelgröße für Heizmatten
// Michael Meiler
// 30.06.2015


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>

// Regelgrößen können hier verstellt werden
// Arbeitspunkt
#define T_WORK 40
// Proportinalbeiwert
#define Kp 3.51
// Integralbeiwert
#define Ki 0.0103
// Abtastzeit
#define Ta 1

// Puffer für Temperatur-Daten
#define BUFFSIZE 35
// Maximale Temperaturabweichung umliegender Punkte des Peak innerhalb der T_matrix
#define PEAK_DIFF_MAX 60
// Minimale Temperaturabweichung umliegender Punkte des Peak innerhalb der T_matrix
#define PEAK_DIFF_MIN 20
// Minimale Temperaturabweichung des Peak im Vergleich zu kältestem Punkt
#define NADIR_DIFF_MIN 20
// Maximaler Wert den der Tiefpunkt besitzen darf
#define NADIR_MAX 240
// Maximaler Wert den Hochpunkt besitzen darf
#define PEAK_MAX 340


// Verwende I2C 1 
#define I2CADDR "/dev/i2c-1"

// Definiert in <linux/i2c-dev.h>
// #define I2C_SLAVE 0x703

// Adresse des D6T-Sensors
#define D6T 0x0a


// Globale Variablen
int		Device;				// Device-Handle
char		Data[BUFFSIZE];		// Datenpuffer
int		tPTAT;				// Umgebungstemperatur
int		tP[16];				// Werte der Pixel als Vektor
int		tPEC;


// Funktion um Daten auszulesen
int D6T_getvalue()
		{
		int i;
			
		//Pointer auf Adresse 0x4c = Startadresse (siehe Datenblatt)
		Data[0] = 0x4C;
		if (write (Device, Data, 1) != 1)
			{
			//printf("Error writing data!\n");	// Einkommentieren zum Debuggen
			return -1;
			}
				
		//Register auslesen
		if (read (Device, Data, BUFFSIZE) != BUFFSIZE)
			{
			//printf("Error reading data!\n");	// Einkommentieren für Fehlersuche
			return -1;
			}
			
		//Werte umwandeln, sind aktuell 10-faches der tatsächlichen Temperatur
		else
			{
			tPTAT = 256*Data[1] + Data[0];
			
			for (i = 0; i < BUFFSIZE; i++)
				{
				tP[i] = 256*Data[((i+1)*2)+1] + Data[(i+1)*2];
				}
						
			tPEC = Data[34];
			return 1;
			}
		}

		
// Funktion um Temperatur in Matrix anzuzeigen mit Farbdarstellung
int D6T_show ()
		{
		int i;
		double Temp[17];
			
		// Werte in tatsächliche Temperatur formatieren
		for (i = 0; i < 16; i++)
			{
			Temp[i] = (double)tP[i]/10;
			}
		Temp[16] = (double)tPTAT/10;
			
		printf("Ambient Temperature: %.2f\n\n", Temp[16]);
		printf("Temp:	");
		// Farbauswahl
		for (i = 0; i < 16; i++)
			{
			if (tP[i] < 180)
				printf("\033[48;5;16m");
			else if (tP[i] < 185)
				printf("\033[48;5;17m");
			else if (tP[i] < 190)
				printf("\033[48;5;18m");
			else if (tP[i] < 195)
				printf("\033[48;5;19m");
			else if (tP[i] < 200)
				printf("\033[48;5;20m");
			else if (tP[i] < 205)
				printf("\033[48;5;21m");
			else if (tP[i] < 210)
				printf("\033[48;5;27m");
			else if (tP[i] < 215)
				printf("\033[48;5;33m");
			else if (tP[i] < 220)
				printf("\033[48;5;39m");
			else if (tP[i] < 225)
				printf("\033[48;5;45m");
			else if (tP[i] < 230)
				printf("\033[48;5;51m");
			else if (tP[i] < 235)
				printf("\033[48;5;50m");
			else if (tP[i] < 240)
				printf("\033[48;5;49m");
			else if (tP[i] < 245)
				printf("\033[48;5;48m");
			else if (tP[i] < 250)
				printf("\033[48;5;47m");
			else if (tP[i] < 255)
				printf("\033[48;5;46m");
			else if (tP[i] < 260)
				printf("\033[48;5;82m");
			else if (tP[i] < 265)
				printf("\033[48;5;118m");
			else if (tP[i] < 270)
				printf("\033[48;5;83m");
			else if (tP[i] < 275)
				printf("\033[48;5;119m");
			else if (tP[i] < 280)
				printf("\033[48;5;154m");
			else if (tP[i] < 285)
				printf("\033[48;5;155m");
			else if (tP[i] < 290)
				printf("\033[48;5;190m");
			else if (tP[i] < 295)
				printf("\033[48;5;226m");
			else if (tP[i] < 300)
				printf("\033[48;5;220m");
			else if (tP[i] < 305)
				printf("\033[48;5;214m");
			else if (tP[i] < 310)
				printf("\033[48;5;208m");
			else if (tP[i] < 315)
				printf("\033[48;5;202m");
			else if (tP[i] < 320)
				printf("\033[48;5;196m");
			else if (tP[i] < 325)
				printf("\033[48;5;197m");
			else if (tP[i] < 330)
				printf("\033[48;5;210m");
			else if (tP[i] < 335)
				printf("\033[48;5;225m");
			else if (tP[i] < 340)
				printf("\033[48;5;230m");
			else
				printf("\033[48;5;231m");
					
			printf(" %.2f ",Temp[i]);
			printf("\033[0m");
			
			// Matrixdarstellung
			if ((i+1) % 4 == 0)
				{
				printf("\n\t");
				if (i == 15)
					printf ("\r\n");
				}
			}
			
		return 1;
		}


int main()
		{
		typedef enum {FALSE = 0, TRUE = 1} boolean_t;
		boolean_t human, power = FALSE;
			
		int i, j, k;
		int T_matrix[4][4];
		int row_peak = 0;
		int col_peak = 0;
		int probability, improbability;
		int heat_power = 0;			// Prozentwert mit welcher Power geheizt werden soll (0-100)
		int control_counter_off = 0, control_counter_on = 0;
		double T_amb,T_wish, e, esum = 0, y;

		
		// Device oeffnen
		if ((Device = open(I2CADDR, O_RDWR)) < 0)
			{
			printf("Can not open I2C-Module!\n");
			return -1;
			}
		
		// Port und Adresse öffnen
		if (ioctl(Device, I2C_SLAVE, D6T) < 0)
			{
			printf("D6T-Address not found!\n");
			return -1;
			}
			
		// Datei zum Schreiben der Daten erstellen
		FILE *datei;
		datei = fopen ("dataset.txt", "w");
		if (datei == NULL)
			{
			printf("Error while opening dataset.txt!");
			return 1;
			}
		fprintf (datei, "T_Ambient\tRow_Peak\tCol_Peak\tValue_Peak\tProbability\tImprobability\tHuman\tHeat_Power\n");
		fclose (datei);
		
		printf("Welche Temperatur wünschen Sie?(in °C): ");
		scanf("%lf", &T_wish);
		printf("\nDanke für die Eingabe\n\n\n");
		sleep(1);
		
		// Hier beginnt die Programm-Schleife
		while (1)
			{
			human = FALSE;
			probability = 0;
			improbability = 0;
			
			do 
				{
				D6T_getvalue();
				}
			while (D6T_getvalue() != 1);
		
			D6T_show();

			// Die Werte in 4x4-Array speichern
			k=0;	
			for (i = 0; i < 4; i++)
				{
				for (j = 0; j < 4; j++)
					{
					T_matrix[i][j] = tP[k + j];
					}
				k += 4;
				}
			
			// Suche höchsten Temperaturwert  = Globales Maximum
			int pos_peak = 0;		
			for (j = 1; j < 16; j++)
				{
				if (tP[pos_peak] < tP[j])
					pos_peak = j;
				}
			row_peak = pos_peak / 4;
			col_peak = pos_peak % 4;
			printf("Peak at: [%d][%d]\n", (row_peak+1), (col_peak+1));
			printf("Peak is: %d\n\n", tP[pos_peak]);
		
			// Suche tiefsten Tempearutrwert = Globales Minimum
			int pos_nadir = 0;
			for (i = 1; i < 16; i++)
				{
				if (tP[pos_nadir] > tP[i])
					pos_nadir = i;
				}
						
			// Untersuche umgebende Werte von Peak
			// Aber nur,wenn Peak kleiner als PEAK_MAX ist
			// Und Peak mindestens um NADIR_DIFF_MIN größer ist als kältester Punkt
			// Und tP[pos_nadir] kälter als NADIR_MAX ist
			if ((tP[pos_peak] <= PEAK_MAX) && (tP[pos_nadir] <= NADIR_MAX) && (tP[pos_peak] - tP[pos_nadir] > NADIR_DIFF_MIN))
				{
				for (i = (row_peak-1); i <= (row_peak+1); i++)
					{
					if (i >= 0 && i <= 3)
						{
						for (j = (col_peak-1); j <= (col_peak+1); j++)
							{	
							if (j >= 0 && j <= 3)
								{	
								// Für jeden Wert dessen Differenz zu tP[pos_peak] zwischen PEAK_DIFF_MAX und PEAK_DIFF_MIN steigt probability um 1
								if (T_matrix[row_peak][col_peak] - T_matrix[i][j] <= PEAK_DIFF_MAX
									&& T_matrix[row_peak][col_peak] - T_matrix[i][j] >= PEAK_DIFF_MIN)
									{
									probability += 1;
									}
								// Für jeden Wert dessen Differenz zu tP[pos_peak] kleiner als PEAK_DIFF_MIN ist steigt improbability um 1
								if (T_matrix[row_peak][col_peak] - T_matrix[i][j] < PEAK_DIFF_MIN
									&& T_matrix[row_peak][col_peak] - T_matrix[i][j] > 0)
									{
									improbability += 1;
									}
							
								//printf("T_matrix[%d][%d]\n", i, j);	// Einkommentieren für Postionen
								//printf("%d\n",  T_matrix[i][j]);		// und deren Werte
								}
							}	
						}
					}
				}	
			//printf("%d %d \n", probability, improbability);				// Einkommentieren zum Debuggen
		
		
			if (probability > 1)
				human = TRUE;
			if (improbability > 1)
				human =FALSE;
			
			
			T_amb = (double)tPTAT/10;			
			// Es muss 3 mal hintereinander ein Mensch erkannt werden damit er einschaltet => Tiefpass		
			if ((human == TRUE) && (control_counter_on <= 3))			// &&-Bedingung, damit er nicht ewig mitzählt
				{
				control_counter_on++;
				if ((control_counter_on >= 3) && (T_wish > T_amb))		// &&-Bedingung, damit nur geheizt wird wenn nötig
					{
					power = TRUE;
					}
				}
			else
				{
				control_counter_on = 0;
				}
			//printf("counter_on: %i\n", control_counter_on);					// Einkommentieren zur Kontrolle
				
			// Wird 10 mal hintereinander kein Mensch erkannt => abschalten => Tiefpass
			if ((human == FALSE) && (control_counter_off <= 10))			// &&-Bedingung, damit er nicht ewig mitzählt
				{
				control_counter_off++;
				if (control_counter_off >= 10)
					{
					power = FALSE;
					esum = 0;								// Summe der Regelabweichung zurücksetzen
					}
				}
			else
				{
				control_counter_off = 0;
				}
			//printf("counter_off: %i\n", control_counter_off);					// Einkommentieren zur Kontrolle
			
			
			// Heizkraft-Regelung, wenn power==True
			if (power)
				{
				e = T_wish - T_amb; 							// Regelabweichung
				esum = esum + e;							// Summe der Regelabweichungen für Integralbildung
				y = T_WORK + Kp * e + Ki * Ta * esum;			// Regelausgangsgröße
				heat_power = y / 43;							// Regelausgangsgröße normieren durch Division durch maximale Temperatur
				}
			else
				{
				heat_power = 0;
				}
			printf("HEATPOWER: %i\n\n",heat_power);					// Einkommentieren zur Kontrolle


			switch (human)
				{
				case FALSE: printf("Human: FALSE\n"); break;
				case TRUE: printf("Human: TRUE\n"); break;
				default: printf("DAFUQ is wrong\n");
				}
				
			
			// Datei zum Schreiben der Daten öffnen und Tabelle schreiben
			datei = fopen("dataset.txt","a");
			if (datei == NULL)
				{
				printf("Error while opening dataset.txt!");
				return 1;
				}				
			fprintf (datei, "%d\t\t\t[%d]\t\t\t[%d]\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t\t%d\t\t\t%d\n", tPTAT, (row_peak+1), (col_peak+1), tP[pos_peak], probability, improbability, human, heat_power);
			fclose (datei);
						
			sleep (1);
			system("clear");
			}
			
			
		return 0;
		}
