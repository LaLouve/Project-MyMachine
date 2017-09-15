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

//PARAMETRAGE BOUTONS
#define BOUTON_PLUS_DROITE    18    //Pin utilisée par le bouton
#define BOUTON_MOINS_DROITE   19
#define BOUTON_PLUS_GAUCHE    20
#define BOUTON_MOINS_GAUCHE   21
#define BOUTON_RESET          3
#define DELAI_REBONDS         100  //En milisecondes

//PARAMETRAGE AFFICHEURS 7 SEGMENTS
#define NOMBRE_AFFICHEURS     4
#define DELAI_RAFRAICHIR      4

//PARAMETRAGE 74LS47
#define NBR_ENTREE_LS         4

//VARAIBLES UTILISÉES PAR LES 74LS47
int EntreeLS[NBR_ENTREE_LS] = {22, 23, 24, 25};  // pin utilisées par les 74ls47
                                                    
//VARIABLES UTILISÉE POUR LES AFFICHEURS
int Anodes[NOMBRE_AFFICHEURS] = {38, 39, 40, 41};
int Nombres[NOMBRE_AFFICHEURS] = {10, 10, 10, 10}; //Nombres à afficher sur chaque afficheur
byte Afficheur = 0; //Numéro du dernier afficheur rafraichi
unsigned long DernierRafraichissement = 0; //Utilisé avec millis()

//VARIABLES UTILISÉES POUR LES BOUTONS
unsigned long DernierAppuisBouton = 0;

//AUTRES VARIABLES
int NombreGauche = 0;
int NombreDroite = 0;


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
  pinMode(BOUTON_PLUS_DROITE, INPUT_PULLUP);
  pinMode(BOUTON_MOINS_DROITE, INPUT_PULLUP);
  pinMode(BOUTON_PLUS_GAUCHE, INPUT_PULLUP);
  pinMode(BOUTON_MOINS_GAUCHE, INPUT_PULLUP);
  pinMode(BOUTON_RESET, INPUT_PULLUP);

  // Configuration des Interruptions
  attachInterrupt(digitalPinToInterrupt(18), AppuisPlusDroite, FALLING);
  attachInterrupt(digitalPinToInterrupt(19), AppuisMoinsDroite, FALLING);
  attachInterrupt(digitalPinToInterrupt(20), AppuisPlusGauche, FALLING);
  attachInterrupt(digitalPinToInterrupt(21), AppuisMoinsGauche, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), AppuisReset, FALLING);

}

void loop() 
{
  RafraichirAffichage();
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

void TransformerNombre ()
{
  int tmpDroite = NombreDroite;
  int tmpGauche = NombreGauche;

  //Comptage des dizaines
  while (tmpGauche >= 10)
  {
    tmpGauche -= 10;
    Nombres[0] = Nombres[0] + 1;
  }
  
  while (tmpDroite >= 10)
  {
    tmpDroite -= 10;
    Nombres[2] = Nombres[2] + 1;
  }
  
  //Comptage des unités
  Nombres[1] = tmpGauche;
  Nombres[3] = tmpDroite;
}

void AppuisBouton (short bouton)
{
  if ((millis() - DernierAppuisBouton) > DELAI_REBONDS)
  {
    DernierAppuisBouton = millis();
    switch(bouton) 
    {
      case 1: NombreDroite++;
              break;
      case 2: NombreDroite--;
              break;
      case 3: NombreGauche++;
              break;
      case 4: NombreGauche--;
              break;
      default:NombreGauche = 0;
              NombreDroite = 0;
              break;
    }
    TransformerNombre();
  }
}

void AppuisPlusDroite ()
{
  AppuisBouton(1);
}

void AppuisMoinsDroite ()
{
  AppuisBouton(2);
}

void AppuisPlusGauche ()
{
  AppuisBouton(3);
}

void AppuisMoinsGauche ()
{
  AppuisBouton(4);
}

void AppuisReset()
{
  AppuisBouton(5);
}
