#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

Servo alt;   // yatay
Servo ust;   // dikey

const int trigPin = 11;
const int echoPin = 12;

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---- DURUM ----
bool oncekiYataydaEngelVardi = false;
bool lcdEngelModunda = false;

// ---- TOPLAMLAR ----
int toplamDikeyAdim = 0;   // D
int toplamYatayAdim = 0;   // Y
float toplamMesafe = 0;
int mesafeSayisi = 0;

// ---- MESAFE ----
float mesafeOku() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long sure = pulseIn(echoPin, HIGH, 25000);
  if (sure == 0) return -1;

  return sure * 0.034 / 2;
}

// ---- LCD ----
void lcdTarama() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tarama");
  lcd.setCursor(0, 1);
  lcd.print("Yapiliyor");
}

void lcdEngel() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Engel bulundu");
  lcd.setCursor(0, 1);
  lcd.print("Taraniyor");
}

// ---- AYIRT ETME ----
String ayirtEt(int D, int Y, float M) {

  // 10 - 20 cm
  if (M >= 5 && M < 20) {
    if(Y >= 14 && Y <= 30) // 25 ?
    {
      if (D > 120 && D < 280) return "BARDAK";
    }
    if(Y >= 10 && Y <= 25)
    {
      if (D > 40 && D <= 152) return "KALEM";
    }
  }

  // 20 - 22 cm
  if (M >= 20 && M <= 22) {
    if(Y >= 16 && Y <= 25)
    {
      if (D > 40 && D < 180) return "BARDAK";
    }
    if(Y >= 4 && Y <= 15)
    {
      if (D > 20 && D <= 100) return "KALEM";
    }
  }

  // 22 - 30 cm
  if (M > 22 && M <= 30) {
    if(Y >= 5 && Y <= 40)
    {
      if (D > 10 && D < 250) return "BARDAK";
    }
    if(Y >= 1 && Y <= 13)
    {
      if (D > 10 && D <= 160) return "KALEM";
    }
  }

  // 31 - 40 cm
 if (M > 30) {
    if(Y > 6 && Y < 40)
    {
      if (D > 20 && D < 200) return "BARDAK";
    }
    if(Y >= 1 && Y <= 15)
    {
      if (D > 1 && D < 100) return "KALEM";
    }
  }

  return "Bilinmiyor";
}

// ---- SETUP ----
void setup() {
  Serial.begin(9600);

  alt.attach(4);
  ust.attach(7);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  lcd.init();
  lcd.backlight();
  lcdTarama();

  alt.write(0);
  ust.write(0);
  delay(1000);
}

// ---- LOOP ----
void loop() {

  for (int y = 0; y <= 180; y += 2) {
    alt.write(y);
    delay(120);

    bool buYataydaEngelVar = false;
    int buYatayDikeySayisi = 0;

    for (int d = 0; d <= 120; d += 8) {
  ust.write(d);
  delay(25);

  float mesafe = mesafeOku();

  // === PROCESSING ICIN GONDER ===
  Serial.print(y);      // alt servo (yatay)
  Serial.print(",");
  Serial.print(d);      // ust servo (dikey)
  Serial.print(",");
  Serial.println(mesafe);

  if (mesafe > 0 && mesafe < 55) {
    buYatayDikeySayisi++;
    toplamMesafe += mesafe;
    mesafeSayisi++;
  }
}


    if (buYatayDikeySayisi >= 3) {
      buYataydaEngelVar = true;
      toplamDikeyAdim += buYatayDikeySayisi;
      toplamYatayAdim++;

      if (!lcdEngelModunda) {
        lcdEngel();
        lcdEngelModunda = true;
      }
    }

// ---- ENGEL BİTTİ ----
if (oncekiYataydaEngelVardi && !buYataydaEngelVar && mesafeSayisi > 0) {

  float ortMesafe = toplamMesafe / mesafeSayisi;
  String sonuc = ayirtEt(toplamDikeyAdim, toplamYatayAdim, ortMesafe);

  // LCD ÜST SATIR: D - Y - M
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("D:");
  lcd.print(toplamDikeyAdim);
  lcd.print(" Y:");
  lcd.print(toplamYatayAdim);

  lcd.setCursor(0, 1);
  lcd.print("M:");
  lcd.print(ortMesafe, 1); // 1 ondalik
  lcd.print(" ");
  lcd.print(sonuc);

  // SERI MONITOR
  Serial.println("---- ENGEL ----");
  Serial.print("D: "); Serial.println(toplamDikeyAdim);
  Serial.print("Y: "); Serial.println(toplamYatayAdim);
  Serial.print("M: "); Serial.println(ortMesafe);
  Serial.print("SONUC: "); Serial.println(sonuc);
  Serial.println("----------------");

  delay(3000);
  lcdTarama();

  // RESET
  toplamDikeyAdim = 0;
  toplamYatayAdim = 0;
  toplamMesafe = 0;
  mesafeSayisi = 0;
  lcdEngelModunda = false;
}


    oncekiYataydaEngelVardi = buYataydaEngelVar;
    ust.write(0);
  }
}
