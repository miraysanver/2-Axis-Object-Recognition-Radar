import processing.serial.*;

Serial myPort;

float altAci = 0;
float ustAci = 0;
float mesafe = 0;

void setup() {
  size(1000, 700);
  background(0);

  println(Serial.list());
  myPort = new Serial(this, "COM3", 9600);
  myPort.bufferUntil('\n');
}

void draw() {
  // iz efekti
  noStroke();
  fill(0, 25);
  rect(0, 0, width, height);

  drawRadar();
  drawRadarFan();
  drawInfo();
}

void serialEvent(Serial p) {
  String s = p.readStringUntil('\n');
  if (s != null) {
    s = trim(s);
    String[] d = split(s, ',');
    if (d.length == 3) {
      altAci = float(d[0]);
      ustAci = float(d[1]);
      mesafe = float(d[2]);
    }
  }
}

void drawRadar() {
  pushMatrix();
  translate(width/2, height - 50);
  stroke(0, 180, 0);
  noFill();

  for (int r = 100; r <= 500; r += 100) {
    arc(0, 0, r*2, r*2, PI, TWO_PI);
  }

  for (int a = 0; a <= 180; a += 30) {
    float rad = radians(180 - a);
    line(0, 0, 500*cos(rad), -500*sin(rad));
  }
  popMatrix();
}

void drawRadarFan() {
  pushMatrix();
  translate(width/2, height - 50);

  float rMax = 500;

  // Arduino 0–120 → Processing 0–180
  float merkezAci = map(ustAci, 0, 120, 0, 180);

  int fanGenislik = 6;     // derece (sık çizgi için)
  float adim = 0.4;        // çizgi sıklığı

  for (float a = merkezAci - fanGenislik; a <= merkezAci + fanGenislik; a += adim) {
    float rad = radians(180 - a);

    float xUc = rMax * cos(rad);
    float yUc = -rMax * sin(rad);

    // 🟢 YEŞİL TARAMA FAN
    stroke(0, 255, 0, 180);
    strokeWeight(2);
    line(0, 0, xUc, yUc);

    // 🔴 KIRMIZI ENGEL FAN
    if (mesafe > 1 && mesafe < 55) {
      float rEngel = map(mesafe, 0, 55, 0, rMax);
      rEngel = constrain(rEngel, 0, rMax);

      float xEngel = rEngel * cos(rad);
      float yEngel = -rEngel * sin(rad);

      stroke(255, 0, 0, 200);
      strokeWeight(6);
      line(xEngel, yEngel, xUc, yUc);
    }
  }

  popMatrix();
}

void drawInfo() {
  fill(255);
  textSize(14);
  text("UST ACI: " + ustAci, 20, 30);
  text("ALT ACI: " + altAci, 20, 50);
  text("MESAFE: " + nf(mesafe, 0, 1) + " cm", 20, 70);
}
