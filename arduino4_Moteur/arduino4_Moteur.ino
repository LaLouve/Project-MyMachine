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
 * arduino n°4: Moteur de la tête
 */

//PARAMETRAGE MOTEUR
#define MOTEUR_SENSA_1        9
#define MOTEUR_SENSA_2        8
#define MOTEUR_SENSB_1        7
#define MOTEUR_SENSB_2        6
#define TEMPS_ROTA            1000    //En milisecondes
#define DELAI_MIN             10      //En secondes
#define DELAI_MAX             30      //En secondes

//VARIABLES POUR LE MOTEUR
unsigned long DernierMoteur = 0;
unsigned long DernierRota = 0;
long DelaiMoteur = (random(DELAI_MIN, DELAI_MAX)) * 1000;
byte EtapeMoteur = 0;

void setup() 
{
  Serial.begin(9600);
  Serial.println(DelaiMoteur);

  // Configuration du moteur
  pinMode(MOTEUR_SENSA_1, OUTPUT);
  pinMode(MOTEUR_SENSA_2, OUTPUT);
  pinMode(MOTEUR_SENSB_1, OUTPUT);
  pinMode(MOTEUR_SENSB_2, OUTPUT);

  digitalWrite(MOTEUR_SENSA_1, HIGH);
  digitalWrite(MOTEUR_SENSA_2, HIGH);
  digitalWrite(MOTEUR_SENSB_1, HIGH);
  digitalWrite(MOTEUR_SENSB_2, HIGH);  

}

void loop() {
  // put your main code here, to run repeatedly:

}

