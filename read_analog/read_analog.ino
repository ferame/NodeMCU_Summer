int button1 = D0; // push button is connected
int button2 = 10;
int temp1 = 0;    // temporary variable for reading the button pin status
int temp2 = 0;

void setup() {
  Serial.begin(9600);
  pinMode(button1, INPUT);   // declare LED as output
  pinMode(button2, INPUT); // declare push button as input
}

void loop() {
  temp1 = digitalRead(button1);
  temp2 = digitalRead(button2);     
     if (temp1 == HIGH) {
      Serial.println("Button 1");
      delay(100);
     }
     else if (temp2 == HIGH) {
      Serial.println("Button 2");
      delay(100);
     }
     else {
      Serial.println("Nothing");
      delay(100);
     }
}
