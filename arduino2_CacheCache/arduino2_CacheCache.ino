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
 * arduino n°2: cache-cache
 */

//PARAMETRAGE BOUTONS
#define BOUTON_PLUS     18    //Pin utilisée par le bouton
#define BOUTON_MOINS    19
#define BOUTON_START    20
#define BOUTON_RESET    21
#define BOUTON_BUZZ     3
#define DELAI_REBONDS   250   //En millisencondes

//PARAMETRAGE AFFICHEURS 7 SEGMENTS
#define NOMBRE_AFFICHEURS     4
#define DELAI_RAFRAICHIR      4

//PARAMETRAGE 74LS47
#define NBR_ENTREE_LS         4

//PARAMETRAGE AUTRE
#define SECONDES_DECOMPTE     30
#define DELAI_SECONDE         1000

//VARAIBLES UTILISÉES PAR LES 74LS47
int EntreeLS[NBR_ENTREE_LS] = {30, 31, 32, 33};  // pin utilisées par le 74ls47, les pins correspondent dans l'ordre: A0, A1, A2 et A3
                                                  
//VARIABLES UTILISÉE POUR LES AFFICHEURS
int Anodes[NOMBRE_AFFICHEURS] = {38, 39, 40, 41}; // pin afficheur 4x7 segment, les pins correspondent aux entrées de l'afficheur dans l'ordre: 12, 9, 8 et 6 (gauche à droite, bas à haut)
int Nombres[NOMBRE_AFFICHEURS] = {0, 0, 0, 0}; //Nombres à afficher sur chaque afficheur
byte Afficheur = 0; //Numéro du dernier afficheur rafraichi
unsigned long DernierRafraichissement = 0; //Utilisé avec millis()

//VARIABLES UTILISÉES POUR LES BOUTONS
unsigned long DernierAppuisBouton = 0;
boolean start = false;

//AUTRES VARIABLES
int NombreEleve = 0;       //Nombre à afficher sur la partie gauche
int SecondesDecompte = SECONDES_DECOMPTE; //Nombre à afficher sur la partie droite
unsigned long DerniereSeconde = 0; 


void setup() 
{ 
  // Tous les Serial servent pour les tests de fonctionnement, à supprimer pour utilisation définitive.
  Serial.begin(9600);
  Serial.print("Start: ");
  Serial.println(start);
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
  pinMode(BOUTON_BUZZ, INPUT_PULLUP);

  // Configuration des interruptions
  attachInterrupt(digitalPinToInterrupt(18), AppuisPlus, FALLING);
  attachInterrupt(digitalPinToInterrupt(19), AppuisMoins, FALLING);
  attachInterrupt(digitalPinToInterrupt(20), AppuisStart, FALLING);
  attachInterrupt(digitalPinToInterrupt(21), AppuisReset, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), AppuisBuzz, FALLING);

  //Mise des afficheurs à leur valeur intiale
  TransformerNombre();  
}

void loop() {
  RafraichirAffichage();

  if (start and (SecondesDecompte > 0))
  {
    if ((millis() - DerniereSeconde) > DELAI_SECONDE)
    {
      SecondesDecompte --;
      VerifierDepassement();
      TransformerNombre();
      DerniereSeconde = millis();
      Serial.print("Secondes: ");
      Serial.println(SecondesDecompte);
    }
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

void VerifierDepassement()
{
  // borne les nombres à afficher entre 0 et 99
  if (NombreEleve < 0)
  {
    NombreEleve = 0;
  }
  else if (NombreEleve > 99)
  {
    NombreEleve = 99;
  }
  if (SecondesDecompte < 0)
  {
    SecondesDecompte = 0;
  }
  else if (SecondesDecompte > 99)
  {
    SecondesDecompte = 99;
  }
}

void TransformerNombre ()
{
  Serial.println("Transformer nombre");
  int tmpEleve = NombreEleve;
  int tmpSeconde = SecondesDecompte;

  Nombres[0] = 0;
  Nombres[2] = 0;
  
  //Comptage des dizaines
  while (tmpEleve >= 10)
  {
    tmpEleve -= 10;
    Nombres[0] = Nombres[0] + 1;
  }

  while (tmpSeconde >= 10)
  {
    tmpSeconde -= 10;
    Nombres[2] = Nombres[2] + 1;
  }
 
  //Comptage des unités
  Nombres[1] =  tmpEleve;
  Nombres[3] = tmpSeconde;

  Serial.print(Nombres[0]);
  Serial.print(Nombres[1]);
  Serial.print(Nombres[2]);
  Serial.println(Nombres[3]);
}

void AppuisBouton (short bouton)
{
  if ((millis() - DernierAppuisBouton) > DELAI_REBONDS)
  {
    DernierAppuisBouton = millis();
    switch(bouton)
    {
      case 1: NombreEleve++;
              break;
      case 2: NombreEleve--;
              break;
      case 3: NombreEleve--;
              break;
      case 4: start = !start;
              Serial.print("Start :");
              Serial.println(start);
              break;
      default: NombreEleve = 0;
               SecondesDecompte = SECONDES_DECOMPTE;
               Serial.print("Reset:");
               Serial.println(SecondesDecompte);
               if (start)
               {
                start != start;
               }
    }
    VerifierDepassement();
    TransformerNombre();
  }
}

void AppuisPlus()
{
  if (not start)
  {
    AppuisBouton(1);
  }
}

void AppuisMoins()
{
  if (not start)
  {
    AppuisBouton(2);
  }
}

void AppuisBuzz()
{
  if (start)
  {
    AppuisBouton(3);
  }
}

void AppuisStart()
{
  AppuisBouton(4);
}

void AppuisReset()
{
  AppuisBouton(5);  
}
