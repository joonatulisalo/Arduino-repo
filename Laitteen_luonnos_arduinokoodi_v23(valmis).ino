#include <LiquidCrystal.h> //Header-tiedosto näytölle
#include <Wire.h> 
const int pin_RS = 8; // LCD-näytön pinnit
const int pin_EN = 9;
const int pin_d4 = 4;
const int pin_d5 = 5;
const int pin_d6 = 6;
const int pin_d7 = 7;
const int pin_BL = 10;
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);


#define echoPin 2 //Ultraäänisensorin pin (käden tunnistus)
#define trigPin 3 //Ultraäänisensorin pin (käden tunnistus)
#define echoPin2 13 //Ultraäänisensorin pin (aineenmittaus)
#define trigPin2 12 //Ultraäänisensorin pin (aineenmittaus)

const int MOTION_PIN = 10; // Pin liitettynä liikkeentunnistimeen
long duration; // Muuttuja ultraäänisensoria varten (käden tunnistus)
int distance; // Muuttuja ultraäänisensoria varten (käden tunnistus)
long duration2; //Muuttuja toista ultraäänisensoria varten (ainemittaus) 
int distance2; //Muuttuja toista ultraäänisensoria varten (ainemittaus)


int times_used0 = 0; // Muuttuja joka laskee desinfiointikerrat

extern volatile unsigned long timer0_millis; // Muuttujat ajastinta varten
extern volatile unsigned long seconds = 0;
extern volatile unsigned long minutes = 0;
extern volatile unsigned long hours = 0;


const int RGB_RED = 42; //RGB-ledin output pinnit
const int RGB_GREEN = 45;
const int RGB_BLUE = 46;


String menuItems[] = {" USER INTERFACE", " MANUFACTURER", " SOCIAL MEDIA", " MAINTENANCE"}; // Valikon nimet ja muuttujat
int readKey; //Määritetään muuttuja joka lukee, mitä näppäintä painetaan
int menuPage = 0; //Määritetään muuttuja valikon sivuille
int maxMenuPages = round (((sizeof(menuItems) / sizeof(String)) / 2)); //Määritetään sivujen maksimimäärä
int cursorPosition = 0; //Määritetään kursorille muuttuja


String subMenuItems[] = {" TIMES USED", " GAUGE", " LAST MAINT...", " RESET TIMER"}; // Alavalikon nimet
int maxSubMenuPages = round (((sizeof(subMenuItems) / sizeof(String)) / 2)); //Määritetään alavalikon sivujen maksimimäärä


byte downArrow[8] = { //Luodaan valikossa käytettävät merkit
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b10101, // * * *
  0b01110, //  ***
  0b00100  //   *
};


byte upArrow[8] = {
  0b00100, //   *
  0b01110, //  ***
  0b10101, // * * *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100  //   *
};


byte menuCursor[8] = {
  B01000, //  *
  B00100, //   *
  B00010, //    *
  B00001, //     *
  B00010, //    *
  B00100, //   *
  B01000, //  *
  B00000  //
};


byte smileyFace[8] = {
  B00000, //  
  B00100, //    *
  B00010, //     *
  B01001, // *    *
  B00001, //      *
  B01001, // *    *   
  B00010, //     *
  B00100  //    *
};


void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);  //Näyttö käyttää 16x2 tilaa
  pinMode(trigPin, OUTPUT); //Määritetään trigPin lähteväksi
  pinMode(echoPin, INPUT); //Määritetään echoPin tulevaksi
  pinMode(trigPin2, OUTPUT); //Määritetään trigPin2 lähteväksi
  pinMode(echoPin2, INPUT); //Määritetään echoPin2 tulevaksi
  pinMode(11, OUTPUT); // Output ultraäänisensoriin liitettyä summeria ja pumppua varten
  pinMode(MOTION_PIN, INPUT_PULLUP); // PIR sensorin output signaali on transistori, joten tarvitaan ylösvetovastus, että signaali ei kellu


  pinMode(RGB_RED, OUTPUT); //Määritetään RGB-ledit lähteviksi
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);

  
  lcd.createChar(0, menuCursor); //Luodaan merkit LCD-näytölle 
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);
  lcd.createChar(3, smileyFace);
}


void loop() {    
  
//LCD-näytön valikon aliohjelmat

   mainMenuDraw(); 
   drawCursor();
   operateMainMenu();
   delay(50);
        }

        
  void mainMenuDraw() { //Aliohjelma kun siirrytään takaisin päävalikkoon
       
    lcd.clear(); //Näyttö tyhjennetään
  lcd.setCursor(1, 0); //Valitaan tulostuksen/merkin paikka
  lcd.print(menuItems[menuPage]);
  lcd.setCursor(1, 1); //Valitaan tulostuksen/merkin paikka
  lcd.print(menuItems[menuPage + 1]);
  if (menuPage == 0) {
    lcd.setCursor(15, 1); //Valitaan tulostuksen/merkin paikka
    lcd.write(byte(2));  //Tulostetaan merkki
  } 
  else if (menuPage > 0 and menuPage < maxMenuPages) {
    lcd.setCursor(15, 1); //Valitaan tulostuksen/merkin paikka
    lcd.write(byte(2));  //Tulostetaan merkki
    lcd.setCursor(15, 0); //Valitaan tulostuksen/merkin paikka
    lcd.write(byte(1));  //Tulostetaan merkki
  } 
  else if (menuPage == maxMenuPages) {
    lcd.setCursor(15, 0); //Valitaan tulostuksen/kursorin paikka
    lcd.write(byte(1));  //Tulostetaan merkki
  }
}


void drawCursor() { //Aliohjelma kursorin liikkumiseen
  
  for (int x = 0; x < 2; x++) { //Määritetään kursori vaihtamaan riviä kun navigoidaan valikossa
    lcd.setCursor(0, x); //Valitaan tulostuksen/kursorin paikka
    lcd.print(" ");
  }
  
    if (menuPage % 2 == 0) {
    if (cursorPosition % 2 == 0) {  //Jos valikon sivu on parillinen ja kursorin sijainti on parillinen, kursorin täytyy olla ensimmäisellä rivillä
      lcd.setCursor(0, 0); //Valitaan tulostuksen/kursorin paikka
      lcd.write(byte(0)); //Tulostetaan merkki
    }
    if (cursorPosition % 2 != 0) {  //Jos valikon sivu on parillinen ja kursorin sijainti on pariton, kursorin täytyy olla toisella rivillä
      lcd.setCursor(0, 1); //Valitaan tulostuksen/kursorin paikka
      lcd.write(byte(0));  //Tulostetaan merkki
    }
  }
  if (menuPage % 2 != 0) {
    if (cursorPosition % 2 == 0) {  //Jos valikon sivu on pariton ja kursorin sijainti on parillinen, kursorin täytyy olla toisella rivillä
      lcd.setCursor(0, 1); //Valitaan tulostuksen/kursorin paikka
      lcd.write(byte(0));  //Tulostetaan merkki
    }
    if (cursorPosition % 2 != 0) { //Jos valikon sivu on pariton ja kursorin sijainti on pariton, kursorin täytyy olla ensimmäisellä rivillä
      lcd.setCursor(0, 0); //Valitaan tulostuksen/kursorin paikka
      lcd.write(byte(0));  //Tulostetaan merkki
    }
  }
}


void operateMainMenu(){ //Aliohjelma päävalikossa navigoimiseen

  analogWrite(RGB_RED, 255); //Näytön ollessa päävalikossa säädetään RGB-valo tiettyyn väriin
  analogWrite(RGB_GREEN, 0);
  analogWrite(RGB_BLUE, 0);
  
  int activeButton = 0;
  while (activeButton == 0) { 
    int button; //Määritetään näppäimelle muuttuja
    readKey = analogRead(0); //Luetaan A0-pinniltä mitä näppäintä painetaan ja tallennetaan se readKey-muuttujaan
    if (readKey < 790) { //Jos A0:n arvo on alle 790, näppäintä on painettu
      delay(100); //Lisätään viive jotta arvo ehtii stabilisoitua
      readKey = analogRead(0); //Luetaan arvo uudelleen viiveen jälkeen
    }
    button = evaluateButton(readKey); //Kutsutaan evaluateButton-funktio ja annettaan sille readKeyn arvo. Tallennetaan se button muuttujaan
    switch (button) {
      case 0: //Kun button palauttaa arvon 0, mitään ei tapahdu
        break;
        
      case 1:  //Tämä case suoritetaan kun painetaan forward-näppäintä
        button = 0;
        switch (cursorPosition) { //Suoritetaan joku näistä caseista, riippuen siitä missä kohtaa kursori on. Eli käyttäjä voi siirtyä tiettyyn alavalikkoon navigoimalla päävalikossa
          case 0:
            menuItem1();
            break;
          case 1:
            menuItem2();
            break;
          case 2:
            menuItem3();
            break;
          case 3:
            menuItem4();
            break;
    }
        activeButton = 1;
        mainMenuDraw();
        drawCursor();
        break;
        
      case 2: //Tämä case suoritetaan kun up-näppäintä painetaan
        button = 0;
        if (menuPage == 0) {
          cursorPosition = cursorPosition - 1; //Kursorin paikka vaihtuu, kun up-näppäintä painetaan
          cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1)); //Rajoitetaan kursorin liikkuminen
        }
        if (menuPage % 2 == 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages); //Rajoitetaan valikon sivu nollan ja valikkojen maksimisivumäärän välille, kun sivu ja kursori ovat parillisia
        }
        if (menuPage % 2 != 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages); //Rajoitetaan valikon sivu nollan ja valikkojen maksimisivumäärän välille, kun sivu ja kursori ovat parittomia
        }
        cursorPosition = cursorPosition - 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));

        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
        
      case 3: //Tämä case suoritetaan kun down-näppäintä painetaan
        button = 0;
        if (menuPage % 2 == 0 and cursorPosition % 2 != 0) { 
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages); //Rajoitetaan valikon sivu nollan ja valikkojen maksimisivumäärän välille, kun sivu on parillinen ja kursori on pariton
        }
        if (menuPage % 2 != 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages); //Rajoitetaan valikon sivu nollan ja valikkojen maksimisivumäärän välille, kun sivu on pariton ja kursori on  parillinen
        }

        cursorPosition = cursorPosition + 1; //Kursorin paikka vaihtuu, kun down-näppäintä painetaan
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1)); //Rajoitetaan kursorin liikkuminen
        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
    }
  }
}


int evaluateButton(int x) { 
  int result = 0;
  if (x < 50) {         //Määritetään näytön näppäimet oikeisiin arvoihin joita A0-pin lukee
    result = 1; // right
  } else if (x < 195) {
    result = 2; // up
  } else if (x < 380) {
    result = 3; // down
  } else if (x < 790) {
    result = 4; // left
  }
  return result;
}


void menuItem1() {  // Tämä suoritetaan kun valitaan päävalikosta ensimmäinen alavalikko
  
  int activeButton = 0;
  while (activeButton == 0){
    
  digitalWrite(trigPin, LOW);  //  Ultraäänisensorin toiminta kun valikon ensimmäinen alavalikko valitaan
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); //Mitataan aika jolloin sensorin pulssi tulee takaisin kohteesta
  distance = duration * 0.034 / 2; //Etäisyys saadaan kaavasta: pulssin aika * äänen nopeus mikrosekuntteina ja senttimetreinä / 2, koska pulssi kulkee edestakaisin 
        if (distance <= 5) // Määritetään ultraäänisensorille ehto. Jos etäisyys on yhtä suuri tai pienempi kuin 5cm, 11-pin muuttuu HIGH-tilaan ja se käynnistää pumpun
        {
          digitalWrite(11, HIGH); //Pumppu päälle
          delay(300); //Optimoidaan annosteltava desinfiointiaineen määrä
          digitalWrite(11, LOW); //Pumppu pois päältä
          lcd.clear(); //Näyttö tyhjennetään
          lcd.setCursor(3, 0); //Valitaan tulostuksen/kursorin paikka
          lcd.print("Thank You!"); //Tulostetaan näytölle teksti, kun ultraäänisensori on havainnut käden
          lcd.setCursor(7, 1); //Valitaan tulostuksen/kursorin paikka
          lcd.write(byte(3)); //Tulostetaan merkki toiselle riville
          times_used0++; //Aina kun ultraäänisensori havaitsee käden laskuri lisää yhden numeron

          analogWrite(RGB_RED, 0);  // Määritetään RGB-valo kun ultraäänisensori havaitsee käden
          analogWrite(RGB_GREEN, 255);
          analogWrite(RGB_BLUE, 0);
          
          delay(2000); // Lisätään hieman viivettä ennen kuin pumppu voi pumpata uudelleen
          lcd.clear(); //Näyttö tyhjennetään
        }
        
        else
        {
          digitalWrite(11, LOW); // Ellei ultraäänisensori havaitse kättä pidetään 11-pin LOW-tilassa
          Serial.print("Pump OFF \t");
        }


    //LIIKESENSORIN TOIMINTA
    
    int proximity = digitalRead(MOTION_PIN); //Laitteen läheisyyteen saapuva henkilö tunnistetaan pinnille 10 (MOTION_PIN) tulevan arvon avulla
        if (proximity == HIGH) //Jos sensorin output menee HIGH-tilaan, liikettä havaitaan ja näytölle tulee haluttu teksti
        {
          lcd.clear(); //Näyttö tyhjennetään
          lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
          lcd.print("Disinfect your");
          lcd.setCursor(1, 1); //Valitaan tulostuksen/kursorin paikka
          lcd.print("hands please.");
          delay(200);

          analogWrite(RGB_RED, 0); //Määritetään RGB-valo silloin kun liikesensori havaitsee liikkeen
          analogWrite(RGB_GREEN, 0);
          analogWrite(RGB_BLUE, 255);
       }
 
        else
        {
          lcd.clear();
          analogWrite(RGB_RED, 255); //Määritetään RGB-valo silloin kun liikesensori ei havaitse liikettä
          analogWrite(RGB_GREEN, 255);
          analogWrite(RGB_BLUE, 255);
        }
        
    int button;
    readKey = analogRead(0); //Luetaan A0-pinniltä, mitä näppäintä painetaan
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    
    button = evaluateButton(readKey);
    switch (button) {
      case 4:  //Tämä case suoritetaan kun back-näppäintä painetaan
        button = 0;
        activeButton = 1;
        break;
    }
  }
}


void menuItem2() { //Tämä suoritetaan kun valitaan päävalikosta toinen alavalikko
  
  int activeButton = 0;

  lcd.clear(); //Näyttö tyhjennetään
  lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
  lcd.print("MANUFACTURED BY");
  lcd.setCursor(1, 1);
  lcd.print("BUNCH OF MONKES");

  analogWrite(RGB_RED, 255); //Näytön ollessa päävalikossa säädetään RGB-valo tiettyyn väriin
  analogWrite(RGB_GREEN, 0);
  analogWrite(RGB_BLUE, 255);

  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    
    if (readKey < 790) {  //Luetaan A0-pinniltä, mitä näppäintä painetaan
      delay(100);
      readKey = analogRead(0);
    }
    
    button = evaluateButton(readKey);
    switch (button) {
      case 4:  //Tämä case suoritetaan kun back-näppäintä painetaan
        button = 0;
        activeButton = 1;
        break;
    }
  }
}


void menuItem3() { //Tämä suoritetaan kun valitaan päävalikosta kolmas alavalikko
  
  int activeButton = 0;

lcd.clear(); //Näyttö tyhjennetään
lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
lcd.print("FOLLOW US ON IG");
lcd.setCursor(1, 1); //Valitaan tulostuksen/kursorin paikka
lcd.print("@bunchofmonkes");

  while (activeButton == 0) {

    analogWrite(RGB_RED, 255); //Määritetään RGB-valo kun käyttäjä on toisessa alavalikossa
    analogWrite(RGB_GREEN, 0);
    analogWrite(RGB_BLUE, 255);
    delay(300);
    analogWrite(RGB_RED, 255);
    analogWrite(RGB_GREEN, 255);
    analogWrite(RGB_BLUE, 0);
    delay(300);
    analogWrite(RGB_RED, 0);
    analogWrite(RGB_GREEN, 255);
    analogWrite(RGB_BLUE, 255);
    delay(300);
    
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {  //Luetaan A0-pinniltä, mitä näppäintä painetaan
      delay(100);
      readKey = analogRead(0);
    }
    
  button = evaluateButton(readKey);
    switch (button) {
      case 4:  // Tämä case suoritetaan kun back-näppäintä painetaan
        button = 0;
        activeButton = 1;
        break;
    }
  }
}


void subMenuDraw() { //Aliohjelma alavalikon hallintaan

  lcd.clear(); //Näyttö tyhjennetään
  lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
  lcd.print(subMenuItems[menuPage]); //Tulostetaan alavalikon kohta [menuPage]
  lcd.setCursor(1, 1); //Valitaan tulostuksen/kursorin paikka
  lcd.print(subMenuItems[menuPage + 1]);
  if (menuPage == 0) {
    lcd.setCursor(15, 1); //Valitaan tulostuksen/merkin paikka
    lcd.write(byte(2)); //Tulostetaan merkki
  } 
  else if (menuPage > 0 and menuPage < maxSubMenuPages) {
    lcd.setCursor(15, 1); //Valitaan tulostuksen/merkin paikka
    lcd.write(byte(2)); //Tulostetaan merkki
    lcd.setCursor(15, 0); //Valitaan tulostuksen/merkin paikka
    lcd.write(byte(1)); //Tulostetaan merkki
  } 
  else if (menuPage == maxSubMenuPages) {
    lcd.setCursor(15, 0); //Valitaan tulostuksen/merkin paikka
    lcd.write(byte(1)); //Tulostetaan merkki
  }
}


  void operateSubMenu(){ //Aliohjelma alavalikon hallintaan

  analogWrite(RGB_RED, 255);
  analogWrite(RGB_GREEN, 0);
  analogWrite(RGB_BLUE, 0);
  
  int activeButton = 0;
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {  //Luetaan A0-pinniltä, mitä näppäintä painetaan
      delay(100);
      readKey = analogRead(0);
    }
    
    button = evaluateButton(readKey);
    switch (button) {
      case 0: //Mitään ei tapahdu jos 0 palautuu
        break;
        
      case 1:  //Tämä case suoritetaan kun forward-näppäintä painetaan
        button = 0;
        switch (cursorPosition) { //Suoritetaan joku näistä caseista, riippuen siitä missä kohtaa kursori on. Eli käyttäjä voi siirtyä tiettyyn alakohtaan navigoimalla alavalikossa
          case 0:
            subMenuItem1();
            break;
          case 1:
            subMenuItem2();
            break;
          case 2:
            subMenuItem3();
            break;
          case 3:
            subMenuItem4();
            break;
    }   
        
        subMenuDraw();
        drawCursor();
        activeButton = 0;
        break;
        
      case 2:
        button = 0; //Up-näppäimellä on samanlainen toimintaperiaate alavalikossa kuin päävalikossa
        if (menuPage == 0) {
          cursorPosition = cursorPosition - 1;
          cursorPosition = constrain(cursorPosition, 0, ((sizeof(subMenuItems) / sizeof(String)) - 1));
        }
        if (menuPage % 2 == 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxSubMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxSubMenuPages);
        }

        cursorPosition = cursorPosition - 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(subMenuItems) / sizeof(String)) - 1));

        subMenuDraw();
        drawCursor();
        activeButton = 0;
        break;
        
      case 3:
        button = 0; //Down-näppäimellä on samanlainen toimintaperiaate alavalikossa kuin päävalikossa
        if (menuPage % 2 == 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxSubMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxSubMenuPages);
        }

        cursorPosition = cursorPosition + 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(subMenuItems) / sizeof(String)) - 1));
        subMenuDraw();
        drawCursor();
        activeButton = 0;
        break;

        case 4:   //Tämä case suoritetaan kun back-näppäintä painetaan (palaaa päävalikon kohtaan 4)
        button = 0;
        activeButton = 1;
        break;
    }
  }
}


void subMenuItem1() { // Tämä suoritetaan kun valitaan alavalikosta ensimmäinen valikko
  
  int activeButton = 0;

  analogWrite(RGB_RED, 0); //RGB-valaistus navigoidessa tähän kohtaan
  analogWrite(RGB_GREEN, 255);
  analogWrite(RGB_BLUE, 0);

  lcd.clear(); //Näyttö tyhjennetään
  lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
  lcd.print("TOTAL OF "); //Tulostetaan näytölle laitteen käyttökertojen määrä
  lcd.setCursor(1, 1); //Valitaan tulostuksen/kursorin paikka
  lcd.print(times_used0); //Laskuri joka lisää yhden numeron aina kun ultraäänisensori havaitsee käden
  lcd.print(" TIMES USED");


  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) { //Luetaan A0-pinniltä, mitä näppäintä painetaan
      delay(100);
      readKey = analogRead(0);
    }
    
  button = evaluateButton(readKey);
    switch (button) {
      case 4:   // Tämä case suoritetaan kun back-näppäintä painetaan
        button = 0;
        activeButton = 1;
        break;
    }
  }
}

void subMenuItem2() { // Tämä suoritetaan kun valitaan alavalikosta toinen valikko
  
  int activeButton = 0;
  lcd.clear();//Näyttö tyhjennetään

  digitalWrite(trigPin2, LOW); //Desipullon mittaaminen pinnasta ultraäänisensorilla
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = duration2 * 0.034 / 2;

  if (distance2>=10) //Lasketaan aineen pinnan korkeus ja muutetaan sen mukaan tulostettavaa tekstiä ja RGB-valoja
  {                  //Tämän avulla nähdään paljon ainetta on jäljellä
    analogWrite(RGB_RED, 255);
    analogWrite(RGB_GREEN, 0);
    analogWrite(RGB_BLUE, 0);

    lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
    lcd.print("0% left.");
    lcd.setCursor(1, 1); //Valitaan tulostuksen/kursorin paikka
    lcd.print("Replace bottle.");
  }     
    else if (distance2>=9)
    {
    analogWrite(RGB_RED, 255);
    analogWrite(RGB_GREEN, 0);
    analogWrite(RGB_BLUE, 0);

    lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
    lcd.print("20% left.");
    lcd.setCursor(1, 1); //Valitaan tulostuksen/kursorin paikka
    lcd.print("Replace bottle.");
    }
    else if (distance2>=8)
    {
    analogWrite(RGB_RED, 255);
    analogWrite(RGB_GREEN, 0);
    analogWrite(RGB_BLUE, 0);
          
    lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
    lcd.print("30% left.");
    lcd.setCursor(1, 1); //Valitaan tulostuksen/kursorin paikka
    lcd.print("Replace bottle.");
    }
    else if (distance2>=7)
    {
    analogWrite(RGB_RED, 255);
    analogWrite(RGB_GREEN, 125);
    analogWrite(RGB_BLUE, 0);
          
    lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
    lcd.print("40% left.");
    lcd.setCursor(1, 1); //Valitaan tulostuksen/kursorin paikka
    lcd.print("Amount low.");
    }
    else if (distance2>=6)
    {
    analogWrite(RGB_RED, 255);
    analogWrite(RGB_GREEN, 125);
    analogWrite(RGB_BLUE, 0);
          
    lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
    lcd.print("50% left.");
    lcd.setCursor(1, 1); //Valitaan tulostuksen/kursorin paikka
    lcd.print("Bottle half full.");
    }
    else if (distance2>=5)
    {
    analogWrite(RGB_RED, 125);
    analogWrite(RGB_GREEN, 255);
    analogWrite(RGB_BLUE, 0);
          
    lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
    lcd.print("60% left.");
    lcd.setCursor(1, 1); //Valitaan tulostuksen/kursorin paikka
    lcd.print("Amount OK.");
    }
    else if (distance2>=4)
    {
    analogWrite(RGB_RED, 125);
    analogWrite(RGB_GREEN, 255);
    analogWrite(RGB_BLUE, 0);
          
    lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
    lcd.print("70% left.");
    lcd.setCursor(1, 1); //Valitaan tulostuksen/kursorin paikka
    lcd.print("Amount OK.");
    }
    else if (distance2>=3)
    {
    analogWrite(RGB_RED, 0);
    analogWrite(RGB_GREEN, 255);
    analogWrite(RGB_BLUE, 0);
          
    lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
    lcd.print("80% left.");
    lcd.setCursor(1, 1); //Valitaan tulostuksen/kursorin paikka
    lcd.print("Amount OK.");
    }
    else if (distance2>=2)
    {
    analogWrite(RGB_RED, 0);
    analogWrite(RGB_GREEN, 255);
    analogWrite(RGB_BLUE, 0);
          
    lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
    lcd.print("90% left.");
    lcd.setCursor(1, 1); //Valitaan tulostuksen/kursorin paikka
    lcd.print("Amount OK.");
    }
    else if (distance2<=1)
    {
    analogWrite(RGB_RED, 0);
    analogWrite(RGB_GREEN, 255);
    analogWrite(RGB_BLUE, 0);
          
    lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
    lcd.print("100% left.");
    lcd.setCursor(1, 1); //Valitaan tulostuksen/kursorin paikka
    lcd.print("Bottle is full.");
    }

  delay(300);

  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {  //Luetaan A0-pinniltä, mitä näppäintä painetaan
      delay(100);
      readKey = analogRead(0);
    }
    
  button = evaluateButton(readKey);
    switch (button) {
      case 4:   // Tämä case suoritetaan kun back-näppäintä painetaan
        button = 0;
        activeButton = 1;
        break;
    }
  }
}


void subMenuItem3() { // Tämä suoritetaan kun valitaan alavalikosta kolmas valikko
  
  int activeButton = 0;

  analogWrite(RGB_RED, 255); //RGB-valaistus navigoidessa tähän kohtaan
  analogWrite(RGB_GREEN, 0);
  analogWrite(RGB_BLUE, 0);

  while (activeButton == 0) {

    seconds = timer0_millis*0.001; //lasketaan sekunteja
    minutes = seconds/60; //lasketaan minuutteja
    hours = minutes/60; //lasketaan tunteja
    
  
    lcd.clear(); //Näyttö tyhjennetään
    lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
    lcd.print("LAST SERVICED:"); //Tulostetaan näytölle ajastin
    lcd.setCursor(1, 1); //Valitaan tulostuksen/kursorin paikka
  
    if(hours < 10){
      lcd.print("0"); //Tulostetaan nolla tuntien eteen jos tunteja kulunut vähemmän kuin 10
    }
    lcd.print(hours); //Tulostetaan tunnit
    lcd.print(":");
    if(minutes < 10){ //Tulostetaan nolla minuuttien eteen jos minuutteja kulunut vähemmän kuin 10
      lcd.print("0");
    }
    lcd.print(minutes); //Tulostetaan minuutit
    lcd.print(" AGO");
    delay(100);
    
      int button;
      readKey = analogRead(0);
      if (readKey < 790) {  //Luetaan A0-pinniltä, mitä näppäintä painetaan
        delay(100);
        readKey = analogRead(0);
      }
    
  button = evaluateButton(readKey);
    switch (button) {
      case 4:  //Tämä case suoritetaan kun back-näppäintä painetaan
        button = 0;
        activeButton = 1;
        break;
    }
  }
}


void subMenuItem4() { // Tämä suoritetaan kun valitaan alavalikosta neljäs valikko
  
  int activeButton = 0;

  analogWrite(RGB_RED, 255); //RGB-valaistus navigoidessa tähän kohtaan
  analogWrite(RGB_GREEN, 0);
  analogWrite(RGB_BLUE, 0);

  noInterrupts (); //Kun käyttäjä valitsee kolmannen alavalikon ajastin nollataan käyttäen keskeytystä
  timer0_millis = 0;
  interrupts ();
  minutes = 0;
  hours = 0;
 
  lcd.clear(); //Näyttö tyhjennetään
  lcd.setCursor(1, 0); //Valitaan tulostuksen/kursorin paikka
  lcd.print("MAINTENANCE"); //Tulostetaan teksti joka näyttää käyttäjälle että ajastin on nollattu
  lcd.setCursor(1, 1); //Valitaan tulostuksen/kursorin paikka
  lcd.print("HAS BEEN RESET"); //Tulostetaan teksti joka näyttää käyttäjälle että ajastin on nollattu

  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {  //Luetaan A0-pinniltä, mitä näppäintä painetaan
      delay(100);
      readKey = analogRead(0);
    }
    
  button = evaluateButton(readKey);
    switch (button) {
      case 4:  //Tämä case suoritetaan kun back-näppäintä painetaan
        button = 0;
        activeButton = 1;
        break;
    }
  }
}


void menuItem4() { //Tämä suoritetaan kun valitaan päävalikosta neljäs alavalikko

  menuPage = 0; //Navigoidessa alavalikkoon asetetaan valikko ensimmäiseen kohtaan, jotta alakohdat tulostuvat oikeisiin kohtiin
  cursorPosition = 0; //Navigoidessa alavalikkoon asetetaan kursori valikon ensimmäiseen kohtaan
  
  subMenuDraw();
  drawCursor();
  operateSubMenu();

  analogWrite(RGB_RED, 255); //RGB-valaistus navigoidessa tähän kohtaan
  analogWrite(RGB_GREEN, 0);
  analogWrite(RGB_BLUE, 0);
  
  delay(50);

int activeButton = 0;
while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {  //Luetaan A0-pinniltä, mitä näppäintä painetaan
      delay(100);
      readKey = analogRead(0);
    }
    
  button = evaluateButton(readKey);
    switch (button) {
      case 4:   // Tämä case suoritetaan kun back-näppäintä painetaan
        button = 0;
        activeButton = 1;
        
        cursorPosition = 3; //Navigoidessa alavalikkosta pois, asetetaan kursori päävalikon neljänteen kohtaan
        menuPage = 2; //Navigoidessa alavalikkoon asetetaan valikko kolmanteen kohtaan, jotta alakohdat tulostuvat oikeisiin kohtiin
        break;
    }
  }
}
