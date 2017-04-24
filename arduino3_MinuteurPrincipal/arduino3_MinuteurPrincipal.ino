/**
 * The MIT License
 * Copyright (c) 2017 Florence Dervaux
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 * Projet My Machine
 * arduino n°1: Compteur de Points
 */

//PARAMETRAGE DES BOUTONS
#define BOUTON_PLUS           18   // Pin utilisées par les boutons
#define BOUTON_MOINS          19
#define BOUTON_START          20
#define BOUTON_RESET          21
#define DELAI_REBONDS         100 //En millisecondes
 
//PARAMETRAGE AFFICHEURS 7 SEGMENTS
#define NOMBRE_AFFICHEURS     4
#define DELAI_RAFRAICHIR      4

//PARAMETRAGE 74LS47
#define NBR_ENTREE_LS         4

//PARAMETRAGE DU MINUTEUR
#define DELAI_SECONDE         1000

//VARAIBLES UTILISÉES PAR LES 74LS47
int EntreeLS[NBR_ENTREE_LS] = {22, 23, 24, 25};  // pin utilisées par les 74ls47
                                                   
//VARIABLES UTILISÉE POUR LES AFFICHEURS
int Anodes[NOMBRE_AFFICHEURS] = {38, 39, 40, 41};
int Nombres[NOMBRE_AFFICHEURS] = {10, 10, 10, 10}; //Nombres à afficher sur chaque afficheur
byte Afficheur = 0; //Numéro du dernier afficheur rafraichi
unsigned long DernierRafraichissement = 0; //Utilisé avec millis()

//VARIABLES UTILISÉES POUR LES BOUTONS
unsigned long DernierAppuisBouton = 0;
boolean start = false;

//VARIABLES UTILISÉES POUR LE MINUTEUR
unsigned int Tick = 90; //Nombre de secondes à décompter (1m30 = 90 pour test)
unsigned int NbrMinutes = 0;
unsigned int NbrSecondes = 0;
unsigned long DernierTick = 0;


void setup() 
{  
  int i = 0;
  int j = 0;
  // Anodes et entrées du ls configurés en sortie
  for (i = 0; i < NOMBRE_AFFICHEURS; i++)
  {
    pinMode(Anodes[i], OUTPUT);
    pinMode(EntreeLS[i], OUTPUT);
  }

  // Configuration des boutons
  pinMode(BOUTON_PLUS, INPUT_PULLUP);
  pinMode(BOUTON_MOINS, INPUT_PULLUP);
  pinMode(BOUTON_START, INPUT_PULLUP);
  pinMode(BOUTON_RESET, INPUT_PULLUP);

  // Configuration des interruptions
  attachInterrupt(digitalPinToInterrupt(18), AppuisPlus, FALLING);
  attachInterrupt(digitalPinToInterrupt(19), AppuisMoins, FALLING);
  attachInterrupt(digitalPinToInterrupt(20), AppuisStart, FALLING);
  attachInterrupt(digitalPinToInterrupt(21), AppuisReset, FALLING);
}

void loop() 
{
  RafraichirAffichage();
  if (start)
  {
    Tick--;
    TransformerNombre(Tick);
  }
}

void RafraichirAffichage()
{
  int i = 0;
  if ((millis() - DernierRafraichissement) > DELAI_RAFRAICHIR / NOMBRE_AFFICHEURS)
  {
    for (i = 0; i < NOMBRE_AFFICHEURS; i++)
    {
      digitalWrite(Anodes[i], LOW);
    }
    digitalWrite(Anodes[Afficheur], HIGH);

    for (i = 0; i < NBR_ENTREE_LS; i++)
    {
      digitalWrite(EntreeLS[i], bitRead(Nombres[Afficheur], i));
    }
    Afficheur = (Afficheur + 1) % NOMBRE_AFFICHEURS;
    DernierRafraichissement = millis();
  }
}

void TransformerNombre(unsigned int aNombre)
{ 
  if (aNombre < 0)
  {
    aNombre = 0;
  }
  
  NbrMinutes = aNombre/60;
  NbrSecondes = aNombre - (NbrMinutes * 60);

  while (NbrMinutes >= 10)
  {
    NbrMinutes -= 10;
    Nombres[0] = Nombres[0] + 1;
  }
  Nombres[1] = NbrMinutes;

  while (NbrSecondes >= 10)
  {
    NbrSecondes -= 10;
    Nombres[2] = Nombres[2] + 1;
  }
  Nombres[3] = NbrSecondes;
}

void AppuisPlus()
{
  if ((millis() - DernierAppuisBouton) > DELAI_REBONDS)
  {
    if (not start)
    {
      Tick++;
      TransformerNombre(Tick);
      DernierAppuisBouton = millis();
    }
  }
}

void AppuisMoins()
{
  if ((millis() - DernierAppuisBouton) > DELAI_REBONDS)
  {
    if (not start)
    {
      Tick--;
      TransformerNombre(Tick);
      DernierAppuisBouton = millis();
    }
  }
}

void AppuisStart()
{
  if ((millis() - DernierAppuisBouton) > DELAI_REBONDS)
  {
    start != start;
    DernierAppuisBouton = millis();
  }
}

void AppuisReset()
{
  if ((millis() - DernierAppuisBouton) > DELAI_REBONDS)
  {
    if (start)
    {
      start != start;
    }
    Tick = 0;
    TransformerNombre(Tick);
    DernierAppuisBouton = millis();
  }
}








