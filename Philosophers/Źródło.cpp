#include <iostream>
#include <windows.h>
#include <random>
#include <ctime>
#include <string>

using namespace std;

//Struktura ilustruj�ca widelec
typedef struct Fork
{
	int number;
	bool isUsed;
	Fork* nextFork;
	Fork* prevFork;
}Fork;

//Struktura ilustruj�ca filozofa
// isEating: true = filozof aktualnie wykorzystuje swoje widelce(zasoby)
// isHungry: true = filozof potrzebuje skorzystać ze swoich widelców
// isEating: współczynnik określający czas (nie)wykorzystywania zasobów przez filozofa. Filozof nie je - zmniejsza się o 1, jeśli je - zwiększa się o 1 na każdy cykl.
//			 Służy zwiększeniu prawdopodobieństwa, że przy długim czasie niewykorzystania filozof zgłosi zapotrzebowanie na zasób oraz, że przy długim czasie wykorzystania zasobu filozof go zwolni.
typedef struct Philosoph
{
	int number;
	Fork* leftFork;
	Fork*  rightFork;
	Philosoph* nextPhilosoph;
	Philosoph* prevPhilosoph;
	bool isEating;
	bool isHungry;
	int eatingTime;
}Philosoph;

//Struktury pomocnicze do odwo�a� do filozof�w i widelc�w
//************
typedef struct PhilHead
{
	Philosoph* head = NULL;
	int lenght = 0;
}PhilHead;

typedef struct ForkHead
{
	Fork* head = NULL;
	int lenght = 0;
}ForkHead;
//************

Philosoph* createPhilosoph(int);
void addPhilosoph(PhilHead*, Philosoph*, ForkHead*);
Fork* createFork(int);
void forkToPhil(Fork*, Philosoph*);
void changeStatus(Fork*);
void addFork(ForkHead*, Fork*);
void showPhil(PhilHead*);
void showFork(ForkHead*);
void takeRightFork(PhilHead*, ForkHead*);

void startEating(Philosoph*);
void stopEating(Philosoph*);
bool canEat(Philosoph*, bool = false);
void checkNeighbours(Philosoph*);
void wait(Philosoph*);
void getFull(Philosoph*);
void getHungry(Philosoph*);
void think(Philosoph*);
bool rollForHunger(Philosoph*);
double getFullness(Philosoph*);

double HUNGER_PROBABILITY = 0.5;
int sleepTime = 1000;
bool DEBUG = false;

int main()
{
	srand(time(0));
	PhilHead* philHead = new PhilHead();
	ForkHead* forkHead = new ForkHead();

	//Utworzenie filozof�w
	Philosoph* filozof1 = createPhilosoph(1);
	Philosoph* filozof2 = createPhilosoph(2);
	Philosoph* filozof3 = createPhilosoph(3);
	Philosoph* filozof4 = createPhilosoph(4);
	Philosoph* filozof5 = createPhilosoph(5);

	//Utworzenie widelc�w
	Fork* widelec1 = createFork(1);
	Fork* widelec2 = createFork(2);
	Fork* widelec3 = createFork(3);
	Fork* widelec4 = createFork(4);
	Fork* widelec5 = createFork(5);

	//Utworzenie listy widelc�w
	addFork(forkHead, widelec1);
	showFork(forkHead);
	addFork(forkHead, widelec2);
	showFork(forkHead);
	addFork(forkHead, widelec3);
	showFork(forkHead);
	addFork(forkHead, widelec4);
	showFork(forkHead);
	addFork(forkHead, widelec5);
	showFork(forkHead);

	//Posadzenie filozof�w przy stole (przypisanie wszystkich do listy)
	//oraz przydzia� widelca w lewej r�ce
	addPhilosoph(philHead, filozof1, forkHead);
	showPhil(philHead);
	addPhilosoph(philHead, filozof2, forkHead);
	showPhil(philHead);
	addPhilosoph(philHead, filozof3, forkHead);
	showPhil(philHead);
	addPhilosoph(philHead, filozof4, forkHead);
	showPhil(philHead);
	addPhilosoph(philHead, filozof5, forkHead);
	showPhil(philHead);

	//Przydzielenie filozofom widelca w prawej r�ce
	takeRightFork(philHead, forkHead);
	showPhil(philHead);

	//Implementacja rozwiązania problemu pięciu filozofów
	int iterator = 1;
	Philosoph* tempPhilosoph = philHead->head;

	while (true)
	{
		if (tempPhilosoph == philHead->head)
		{
			cout << "\n* * * * * * * * *  Obieg " << iterator << " * * * * * * * * *\n";
			iterator++;
			Sleep(sleepTime);
		}

		// Wylosowanie, czy filozof potrzebuje w tym cyklu dostępu do swoich zasobów
		if (rollForHunger(tempPhilosoph))
			getHungry(tempPhilosoph);
		else
			tempPhilosoph->isHungry = false;

		//Jeśli filozof nie potrzebuje zasosóbów, ale je wykorzystuje - zwolnić zasoby
		if (!tempPhilosoph->isHungry && tempPhilosoph->isEating)
		{
			// zwolnij zasoby
			getFull(tempPhilosoph);
			stopEating(tempPhilosoph);
		}
		else if (tempPhilosoph->isHungry && !tempPhilosoph->isEating)
		{
			// jeśli zasoby są wolne, zajmij je - w p.p. oczekuj na zwolnienie
			if (canEat(tempPhilosoph))
				startEating(tempPhilosoph);
			else
				wait(tempPhilosoph);
		}
		else if (!tempPhilosoph->isHungry && !tempPhilosoph->isEating)
		{
			// praca własna procesu filozofa
			think(tempPhilosoph);
		}
		else if (tempPhilosoph->isHungry && tempPhilosoph->isEating)
		{
			// wykorzystanie zasobów przez filozofa
			tempPhilosoph->eatingTime++;
			cout << "Filozof " << tempPhilosoph->number << " je.";
			if (DEBUG)
				cout << " (Najedzenie = " << getFullness(tempPhilosoph) << ")";
			cout << "\n";
			Sleep(sleepTime);
		}
		tempPhilosoph = tempPhilosoph->nextPhilosoph;
	}

	system("PAUSE");
	return 0;
}

//Wzięcie prawego widelca
void takeRightFork(PhilHead* philHead, ForkHead* forkHead)
{
	Philosoph* tempPhil = philHead->head;
	Fork* tempFork = forkHead->head;
	tempFork = tempFork->nextFork;

	for (int i = 0; i < philHead->lenght; i++)
	{
		forkToPhil(tempFork, tempPhil);

		tempPhil = tempPhil->nextPhilosoph;
		tempFork = tempFork->nextFork;
	}
}

void showPhil(PhilHead* head)
{
	Philosoph* temp = head->head;

	cout << "\n*****\n";
	for (int i = 0; i < head->lenght; i++)
	{
		cout << "philosoph: " << temp << endl;
		cout << "number: " << temp->number << endl;
		cout << "leftFork: " << temp->leftFork << endl;
		cout << "rightFork: " << temp->rightFork << endl;
		cout << "nextPhilosoph: " << temp->nextPhilosoph << endl;
		cout << "prevPhilosoph: " << temp->prevPhilosoph << endl;
		cout << "head->lenght: " << head->lenght << endl;

		temp = temp->nextPhilosoph;
	}
	cout << "\n*****\n";
}

void showFork(ForkHead* head)
{
	Fork* temp = head->head;

	cout << "\n*****\n";
	for (int i = 0; i < head->lenght; i++)
	{
		cout << "fork: " << temp << endl;
		cout << "number: " << temp->number << endl;
		cout << "isUsed: " << temp->isUsed << endl;
		cout << "nextFork: " << temp->nextFork << endl;
		cout << "prevFork: " << temp->prevFork << endl;
		cout << "head->lenght: " << head->lenght << endl;

		temp = temp->nextFork;
	}
	cout << "\n*****\n";
}

//Dodaje widelec do puli widelc�w dost�pnych
void addFork(ForkHead* head, Fork* fork)
{
	if (head->head == NULL)
	{
		head->head = fork;
		head->lenght++;
	}
	else if (head->lenght == 1)
	{
		Fork* temp = head->head;
		temp->nextFork = fork;
		temp->prevFork = fork;
		fork->nextFork = temp;
		fork->prevFork = temp;

		head->lenght++;
	}
	else
	{
		Fork* temp = head->head;
		Fork* temp2 = temp->prevFork;

		temp2->nextFork = fork;
		fork->prevFork = temp2;
		fork->nextFork = temp;
		temp->prevFork = fork;

		head->lenght++;
	}
}

// Funckaj wyznacza mnożnik (nie)najedzenia filozofa, wartość ujemna zwiększa prawdopodobieństwo zgłoszenia zapotrzebowania na zasób
double getFullness(Philosoph* philosoph)
{
	return (double)philosoph->eatingTime / 5;
}

// wylosowanie, czy filozof będzie potrzebował zasobów
bool rollForHunger(Philosoph* philosoph)
{
	double fullness = getFullness(philosoph);
	double rValue = ((double)rand()) / RAND_MAX;

	//wartość (nie)najedzenia jest większa, niż prawdopodobieństwo, że filozof będzie głodny - zwraca true (filozof potrzebuje zasobów)
	bool hungry = ((rValue - fullness) >= HUNGER_PROBABILITY);
	string success = hungry ? " *porazka*" : " *sukces*";
	if (DEBUG)
		cout << "Filozof " << philosoph->number << ": Rzut na nieglod: (" << rValue << " - " << fullness << ") <= " << HUNGER_PROBABILITY << success << "\n";
	Sleep(sleepTime);
	return hungry;
}

//Zmienia stan widelca wolny/zaj�ty
void changeStatus(Fork* fork)
{
	fork->isUsed = !fork->isUsed;
}

// zgłoszenie zapotrzebowania na zasób
void getHungry(Philosoph* philosoph)
{
	if (!philosoph->isHungry)
	{
		philosoph->isHungry = true;
		cout << "Filozof " << philosoph->number << " zrobil sie glodny.\n";
		Sleep(sleepTime);
	}
}

// zgłoszenie chęci zwolnienia zasobów
void getFull(Philosoph* philosoph)
{
	philosoph->isHungry = false;
	philosoph->eatingTime = 0;
	cout << "Filozof " << philosoph->number << " sie najadl.\n";
	Sleep(sleepTime);
}

// praca własna procesu filozofa
void think(Philosoph* philosoph)
{
	philosoph->eatingTime--;
	cout << "Filozof " << philosoph->number << " rozmysla.";
	if (DEBUG)
		cout << " (Najedzenie = " << getFullness(philosoph) << ")";
	cout << "\n";
	Sleep(sleepTime);
}

// zajęcie zasobów przez filozofa
void startEating(Philosoph* philosoph)
{
	philosoph->leftFork->isUsed = true;
	philosoph->rightFork->isUsed = true;
	philosoph->isEating = true;
	philosoph->eatingTime++;

	cout << "Filozof " << philosoph->number << " wzial lewy widelec.\n";
	Sleep(sleepTime);
	cout << "Filozof " << philosoph->number << " wzial prawy widelec.\n";
	Sleep(sleepTime);
	cout << "Filozof " << philosoph->number << " zaczal jesc.\n";
	Sleep(sleepTime);
}

// zwolnienie zasobów przez filozofa
void stopEating(Philosoph* philosoph)
{
	if (philosoph->isEating)
	{
		philosoph->leftFork->isUsed = false;
		philosoph->rightFork->isUsed = false;
		philosoph->isEating = false;

		cout << "Filozof " << philosoph->number << " skonczyl jesc i odlozyl widelce.\n";
		Sleep(sleepTime);
	}
}

// sprawdzenie, czy filozof może zająć zasoby
bool canEat(Philosoph* philosoph, bool repeatedCheck)
{
	if (!repeatedCheck)
	{
		if (philosoph->leftFork->isUsed)
		{
			cout << "Filozof " << philosoph->number << " nie moze wziac lewego widelca.\n";
			Sleep(sleepTime);
		}
		if (philosoph->rightFork->isUsed)
		{
			cout << "Filozof " << philosoph->number << " nie moze wziac prawegoo widelca.\n";
			Sleep(sleepTime);
		}
	}


	return !(philosoph->leftFork->isUsed || philosoph->rightFork->isUsed);
}

// oczekiwanie filozofa na zwolnienie zasobów
void wait(Philosoph* philosoph)
{
	philosoph->eatingTime--;
	cout << "Filozof " << philosoph->number << " musi zaczekac na swoja kolej.\n";
	Sleep(sleepTime);
}

//Przypisanie widelca do filozofa
void forkToPhil(Fork* fork, Philosoph* philosoph)
{
	if (philosoph->leftFork == NULL)
	{
		philosoph->leftFork = fork;
	}
	else
	{
		philosoph->rightFork = fork;
	}
}

//Tworzenie widelca i przypisanie mu numeru
Fork* createFork(int number)
{
	Fork* fork = new Fork();
	fork->number = number;
	fork->isUsed = false;
	fork->nextFork = NULL;
	fork->prevFork = NULL;

	return fork;
}

//Tworzenie filozofa i przypisanie mu numeru
Philosoph* createPhilosoph(int number)
{
	Philosoph* philosoph = new Philosoph();
	philosoph->number = number;
	philosoph->leftFork = NULL;
	philosoph->rightFork = NULL;
	philosoph->nextPhilosoph = NULL;
	philosoph->prevPhilosoph = NULL;
	philosoph->isEating = false;

	return philosoph;
}

//Dodaje filozofa do listy filozof�w i przypisuje mu widelec
void addPhilosoph(PhilHead* philHead, Philosoph* philosoph, ForkHead* forkHead)
{
	if (philHead->head == NULL)
	{
		philHead->head = philosoph;
		philHead->lenght++;

		Fork* temp = forkHead->head;

		if (temp->number == philosoph->number)
			forkToPhil(temp, philosoph);
		else
			cout << "B�edne przypisanie widelca\n";
	}
	else if (philHead->lenght == 1)
	{
		Philosoph* temp = philHead->head;
		temp->nextPhilosoph = philosoph;
		temp->prevPhilosoph = philosoph;
		philosoph->nextPhilosoph = temp;
		philosoph->prevPhilosoph = temp;

		philHead->lenght++;

		Fork* temp2 = forkHead->head;
		temp2 = temp2->nextFork;

		if (temp2->number == philosoph->number)
			forkToPhil(temp2, philosoph);
		else
			cout << "B�edne przypisanie widelca\n";
	}
	else
	{
		Philosoph* temp = philHead->head;
		Philosoph* temp2 = temp->prevPhilosoph;

		temp2->nextPhilosoph = philosoph;
		philosoph->prevPhilosoph = temp2;
		philosoph->nextPhilosoph = temp;
		temp->prevPhilosoph = philosoph;

		philHead->lenght++;

		Fork* temp3 = forkHead->head;

		for (int i = 1; i < philosoph->number; i++)
		{
			temp3 = temp3->nextFork;
		}

		if (temp3->number == philosoph->number)
			forkToPhil(temp3, philosoph);
		else
			cout << "B�edne przypisanie widelca\n";
	}
}