//Assume when my longs overflow, the board just reboots
int lightSensor1 = A7;
int lightSensor2 = A6; 
int val = 0;
int ONTHRESH = 50;
int state = 0; 
bool on = false;
unsigned long timer = 0;
unsigned long aTimer = 0;
int count = 0;
unsigned long ONTIME = 0;
unsigned long lowThreshRst = 3600000; //1 hour

bool debug = false;

int avg(int times){ // just to get an average of the sensor value
  int total = 0;
  for(int i = 0; i < times; i++){
    total += analogRead(lightSensor2); // for sign one A2 , sign two A1
  }
  return total/times;
}

void newThresh(){ //redefines the onthresh since ambient light can change
  digitalWrite(13, LOW);
  int loops = 0; //Loop is there because I thought the staying-on bug was caused by getting trapped in this loop
  while(avg(1) > ONTHRESH || loops > 500){ //has to be off so values don't get messed up
    loops++;
    delay(100);
  }
  ONTHRESH = avg(50) + 10;
}



void setup() {
  if(debug){
    Serial.begin(115200); 
    Serial.println("Hello"); 
  }
  
  pinMode(13, OUTPUT);        //  setup serial
  digitalWrite(13, HIGH); //Just to show it has rebooted properly
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);
  newThresh(); //defines the onthresh
}

void loop() {
  delay(200);
  if(millis() > lowThreshRst){ //redefines every one hour
    newThresh();
    lowThreshRst += lowThreshRst;
  }

  int Tstate = 0;
  val = avg(1);  // read the input pin

  if(val > ONTHRESH){
    Tstate = 1;
    if(state == 0){
      timer = millis();
      aTimer = millis();
    }
    
  }
  else if(val < ONTHRESH){
    Tstate = 0;
  }

  if(debug){
    Serial.println(String(ONTHRESH)+","+String(val));
    Serial.println(millis() - timer); 
    Serial.println(String(state)+","+String(Tstate));
  }
  

  if( state == 1 && Tstate == 0 && (millis() - timer > 400) && (millis() - timer < 800)){ //Flashing 
    count++;
    aTimer = millis();
    if(debug){
      Serial.println(millis() - timer);
      Serial.println("COUNT:"+String(count));
    }
  }
  

  if(count >= 2){ // only after the second flash turn on the light (mitigate false readings)
    digitalWrite(13, HIGH);
    if(!on){
      ONTIME = millis();
    }
    on = true;
  }

  if((millis()-ONTIME) > 15000 && on){ // turn off if on for more than 15 sec
    digitalWrite(13, LOW);
    on = false;
    count = 0;
  }

  if((val < ONTHRESH && (millis() - aTimer) > 2000) || (state == 1 && Tstate == 1 && (millis() - aTimer) > 1500) ){
    //turns off the blue light if the panel turns off or if the LEDs stop flashing
    digitalWrite(13, LOW);
    on = false;
    count = 0;
  }

  if(digitalRead(13) == HIGH){
    on = true;
  }
  
  state = Tstate; 

}
