

int lightSensor1 = A7;
int lightSensor2 = A6; 
int val = 0;
int LOWTHRESH = 2; 
int HIGHTHRESH = 200; 
int state = 0; 
bool on = false;
unsigned long timer = 0;
unsigned long aTimer = 0;
int count = 0;
unsigned long ONTIME = 0;
unsigned long lowThreshRst = 7200000; //2 hours

int avg(int times){
  int total = 0;
  for(int i = 0; i < times; i++){
    total += analogRead(lightSensor1);
  }
  //Serial.println((total/50 + 8));
  return total/times;
}

void newThresh(){
  while(avg(1) > HIGHTHRESH){

  }
  LOWTHRESH = avg(50) + 10;
  HIGHTHRESH = LOWTHRESH + 15;
}



void setup() {
  Serial.begin(115200); 
  //Serial.println("Hello");  
  pinMode(13, OUTPUT);        //  setup serial
  digitalWrite(13, LOW);
  delay(1000);
  newThresh();
}

void loop() {
  delay(200);
  if(millis() > lowThreshRst){
    newThresh();
    lowThreshRst += lowThreshRst;
  }

  int Tstate = 0;
  val = avg(1);  // read the input pin
  Serial.println(String(LOWTHRESH)+","+String(val)+","+String(HIGHTHRESH));
  if(val > HIGHTHRESH){
    Tstate = 1;
    if(state == 0){
      timer = millis();
      aTimer = millis();
    }
    
  }
  else if(val < LOWTHRESH){
    Tstate = 0;
  }
  Serial.println(millis() - timer);
  Serial.println(String(state)+","+String(Tstate));
  if( state == 1 && Tstate == 0 && (millis() - timer > 400) && (millis() - timer < 800)){
    Serial.println(millis() - timer);
    count++;
    aTimer = millis();
    Serial.println("COUNT:"+String(count));
  }
  
  

  if(count >= 2){
    digitalWrite(13, HIGH);
    if(!on){
      ONTIME = millis();
    }
    on = true;
  }

  if((millis()-ONTIME) > 10000 && on){
    digitalWrite(13, LOW);
    on = false;
    count = 0;
  }

  if((val < LOWTHRESH && (millis() - aTimer) > 2000) || (state == 1 && Tstate == 1 && (millis() - aTimer) > 1500) ){
    digitalWrite(13, LOW);
    on = false;
    count = 0;
  }
  
  state = Tstate;


}