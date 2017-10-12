#include <Keypad.h>
#include <Key.h>
/**
 * The MIT License
 * Copyright (c) 2017 Florence Dervaux
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, Modify, merge, publish, distribute, sublicense, and/or sell
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
 * arduino n°3: Minuteur principal 
 */

//PARAMETRAGE DES BOUTONS
#define BOUTON_START          18
#define SWITCH_CLEF           19   // ATTENTION LOGIQUE INVERSE, clef activé à 0. clé connectée à 19 et masse
#define DELAI_REBONDS         250 //En millisecondes
 
//PARAMETRAGE AFFICHEURS 7 SEGMENTS
#define NOMBRE_AFFICHEURS     4
#define DELAI_RAFRAICHIR      4

//PARAMETRAGE 74LS47
#define NBR_ENTREE_LS         4

//PARAMETRAGE DU MINUTEUR
#define DELAI_SECONDE         1001

//PARAMETRAGE CLAVIER
 #define LIGNES         4   //4 LIGNES
 #define COLONNES       4   //4 COLONNES

//VARAIBLES UTILISÉES PAR LES 74LS47
int EntreeLS[NBR_ENTREE_LS] = {30, 31, 32, 33};  // pin utilisées par le 74ls47, les pins correspondent dans l'ordre: A0, A1, A2 et A3
                                                   
//VARIABLES UTILISÉE POUR LES AFFICHEURS
int Anodes[NOMBRE_AFFICHEURS] = {38, 39, 40, 41}; // pin afficheur 4x7 segment, les pins correspondent aux entrées de l'afficheur dans l'ordre: 12, 9, 8 et 6 (gauche à droite, bas à haut)
int Nombres[NOMBRE_AFFICHEURS] = {0, 0, 0, 0}; //Nombres à afficher sur chaque afficheur
byte Afficheur = 0; //Numéro du dernier afficheur rafraichi
unsigned long DernierRafraichissement = 0; //Utilisé avec millis()

//VARIABLES UTILISÉES POUR LES BOUTONS
unsigned long DernierAppuisBouton = 0;
boolean Start = false;
boolean Clef = false;

//VARIABLES UTILISÉES POUR LE MINUTEUR
unsigned int Tick = 0; //Nombre de secondes à décompter (1m30 = 90 pour test)
unsigned int NbrMinutes = 0;
unsigned int NbrSecondes = 0;
unsigned long DernierTick = 0;

//VARIABLES CLAVIER
char symboles[LIGNES][COLONNES] =  {{'1', '4', '7', '*'},
                                    {'2', '5', '8', '0'},
                                    {'3', '6', '9', '#'},
                                    {'A', 'B', 'C', 'D'}};
byte pinLignes[LIGNES] = {9, 8, 7, 6};
byte pinColonnes[COLONNES] = {13, 12, 11, 10};
boolean Modif = false;
byte AfficheurModif = 0;

int i = 0;

//initialisation du clavier avec la bibliothèque keypad
Keypad clavier = Keypad(makeKeymap(symboles), pinLignes, pinColonnes, LIGNES, COLONNES);

void setup() 
{ 
  Serial.begin(9600); 
  int i = 0;
  int j = 0;
  // Anodes et entrées du ls configurés en sortie
  for (i = 0; i < NOMBRE_AFFICHEURS; i++)
  {
    pinMode(Anodes[i], OUTPUT);
    pinMode(EntreeLS[i], OUTPUT);
  }

  // Configuration des boutons
  pinMode(BOUTON_START, INPUT_PULLUP);
  pinMode(SWITCH_CLEF, INPUT_PULLUP);

  // Configuration des interruptions

  attachInterrupt(digitalPinToInterrupt(BOUTON_START), AppuisStart, FALLING);
}

void loop() 
{
  RafraichirAffichage();
  
  Clef = digitalRead(SWITCH_CLEF);
  if (!Clef)
  {
    AppelClavier();
  }

  if (Start and (Tick > 0))
  {
    if ((millis() - DernierTick) > DELAI_SECONDE)
    {
      Tick--;
      BornageTick();
      TransformerNombre();
      DernierTick = millis();
    }
  }
}

void RafraichirAffichage()
{
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

void AppelClavier()
{
  char caractere = clavier.getKey();

  if (caractere == '*')
  {
    //Serial.println(caractere);
    Modif = true;
    Start = false;
    AfficheurModif = 0;
    for (i = 0; i < NOMBRE_AFFICHEURS; i++)
    {
      // permet de ne rien afficher sans éteindre l'afficheur
      Nombres[i] = 15;
    }
  }
  else if ((Modif) && (isDigit(caractere)))
  {
    String convChiffre = "";
    convChiffre += caractere;
    int chiffre = convChiffre.toInt();

    if ((AfficheurModif == 2) and (chiffre > 5))
    {
      chiffre = 5;
    }
    
    Nombres[AfficheurModif] = chiffre;
    
    RafraichirAffichage();
    if (AfficheurModif == 3)
    {
      Modif = false;
      TransfoTick();
    }
    AfficheurModif = (AfficheurModif + 1) % NOMBRE_AFFICHEURS;
  }
}

void BornageTick()
{
  if (Tick > 5999)
  {
    Tick = 5999;
  }
  else if (Tick < 0)
  {
    Tick = 0;
  }
}

void TransfoTick()
{
  NbrMinutes = (Nombres[0] * 10) + Nombres[1];
  NbrSecondes = (Nombres[2] * 10) + Nombres[3];
  Tick = (NbrMinutes * 60) + NbrSecondes;
  BornageTick();
}

void TransformerNombre()
{ 
  NbrMinutes = Tick/60;
  NbrSecondes = Tick - (NbrMinutes * 60);

  Nombres[0] = 0;
  Nombres[2] = 0;

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

  Serial.println(" Affichage: ");
  Serial.println(Nombres[0]);
  Serial.println(Nombres[1]);
  Serial.println(Nombres[2]);
  Serial.println(Nombres[3]);
}

void AppuisStart()
{
  if ((millis() - DernierAppuisBouton) > DELAI_REBONDS)
  {
    Start = !Start;
    DernierAppuisBouton = millis();
  }
}
