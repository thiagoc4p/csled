const int pin = 8;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(100);
  pinMode(pin, OUTPUT);
}
int serIn;
void loop () {
  
  if(Serial.available() > 0) {
    Serial.flush();   

        serIn = Serial.read();    
        Serial.print(serIn);
        if(serIn == '1') {
          digitalWrite(pin, HIGH);

        } 
        else if (serIn == '0') {
            digitalWrite(pin, LOW);

        }
  }
}
