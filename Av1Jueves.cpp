#include <Servo.h>
#include <PID_v1.h>//libreria PID
//MPU6050
//PINES: SCL-21,SDA-20
#include "Simple_MPU6050.h"
#define MPU6050_ADDRESS_AD0_LOW     0x68 
#define MPU6050_ADDRESS_AD0_HIGH    0x69 
#define MPU6050_DEFAULT_ADDRESS     MPU6050_ADDRESS_AD0_LOW
#define Three_Axis_Quaternions 3
#define Six_Axis_Quaternions 6  
Simple_MPU6050 mpu(Six_Axis_Quaternions);
#define spamtimer(t) for (static uint32_t SpamTimer; (uint32_t)(millis() - SpamTimer) >= (t); SpamTimer = millis()) 
#define printfloatx(Name,Variable,Spaces,Precision,EndTxt) print(Name); {char S[(Spaces + Precision + 3)];Serial.print(F(" ")); Serial.print(dtostrf((float)Variable,Spaces,Precision ,S));}Serial.print(EndTxt);
float z_rotation;//angulo de rotacion
float angulo;
float ypr[3] = { 0, 0, 0 };
float xyz[3] = { 0, 0, 0 };
//todos los pines nombrados con numero par son lado derecho e impares izquierdp
//Motores
const int ENA_MT1 = 10;//atras derecha
const int ENA_MT2 = 5;//atras izq
const int ENA_MT3 = 6;//adelante deracha
const int ENA_MT4 = 7;//adelante izq
const int IN1_MT = 26;//atras der
const int IN2_MT = 27;
const int IN3_MT = 28;//atras izq
const int IN4_MT = 29;

const int IN5_MT = 30;//adelante deracha
const int IN6_MT = 31;
const int IN7_MT = 32;//adelante izq
const int IN8_MT = 33;
int SPEED_MT=170;//velocidad inicial
int SPEED_MT2=170;
//servo
Servo servo;
//encoders-variables de control
const int encoder=19; // der
const int encoder2=18;//izq
int c;//contador de pulsos der
int c2;//contador de pulsos izq
int cplus;//contador de pulsos der
int cplus2;//contador de pulsos izq
int frenado;//avance del motor en el frenado
int frenado2;
int error_giro=8;
// Variables para PID
double setpoint; // Valor deseado de velocidad
double input;    // Valor actual de velocidad
double output;   // Salida del PID
unsigned long time1;

// Parámetros del PID
double Kp = 1; // Ganancia proporcional
double Ki = 2; // Ganancia integral
double Kd = 0.01; // Ganancia derivativa

// Inicializa el PID
PID myPID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);
//ultrasonico
const int trig = 12;
const int echo = 13;
long duration;
long distance;

//sensor de color
const int s0=34;
const int s1=35;
const int s2=36;
const int s3=37;
const int outTCS=38;
// infrarrojo
const int infrared1=24;//der
const int infrared2=25;//izq

int dt=500;
int orientacion=0;
int m;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //motores
  pinMode(IN1_MT,OUTPUT);
  pinMode(IN2_MT,OUTPUT);
  pinMode(IN3_MT,OUTPUT);
  pinMode(IN4_MT,OUTPUT);
  pinMode(IN5_MT,OUTPUT);
  pinMode(IN6_MT,OUTPUT);
  pinMode(IN7_MT,OUTPUT);
  pinMode(IN8_MT,OUTPUT);
  pinMode(ENA_MT1,OUTPUT);
  pinMode(ENA_MT2,OUTPUT);
  pinMode(ENA_MT3,OUTPUT);
  pinMode(ENA_MT4,OUTPUT);
  analogWrite(ENA_MT1,SPEED_MT);
  analogWrite(ENA_MT2,SPEED_MT2);
  analogWrite(ENA_MT3,SPEED_MT);
  analogWrite(ENA_MT4,SPEED_MT2);
  //servo
  servo.attach(11);
  //encoder
  pinMode(encoder,INPUT);
  pinMode(encoder2,INPUT);
  attachInterrupt(4,interruption,RISING);//interrupcion para contar pulsos del encoder
  attachInterrupt(5,interruption2,RISING);
  //ultrasonico
  pinMode(trig, OUTPUT);  
  pinMode(echo, INPUT); 
  //sensor de color
  pinMode(s0,OUTPUT);
  pinMode(s1,OUTPUT);
  pinMode(s2,OUTPUT);
  pinMode(s3,OUTPUT);
  pinMode(outTCS,INPUT);
  digitalWrite(s0,1);
  digitalWrite(s1,0); 
    //PID
  myPID.SetMode(AUTOMATIC);
  inicializarMPU6050();
  pinMode(infrared1,INPUT);
  pinMode(infrared2,INPUT);
}
void loop() {

  // float angulo=getAngulo();
  // Serial.println(angulo);
  right();
  stop(1500);

}
// void ajustar_velocidad(){
//   time1=millis();
//   while(millis()-time1<4000);
//   int kvar=5;
//   SPEED_MT=speed+5(8-c);
//   speed2=speed2+5(8-c2);
//   c=0;
//   c2=0;
//   delay(200);
// }
//funcion contabilizadora de pulsos del encoder
void interruption() {
  cplus++;
  c++;
  Serial.println(c);
}
void interruption2() {
  cplus2++;
  c2++;
  Serial.println(c2);
}
int corregir_giro(){
  int kerror=14;
  int corregir;
  loop_mpu();
  if(z_rotation<=-179.9 && z_rotation>-170){
    corregir=kerror*(z_rotation+orientacion);
  }
  else{
    corregir=kerror*(z_rotation-orientacion);
  }
  return corregir;
}
bool lineaAbajo(){
  int state_infra1=digitalRead(infrared1);
  int state_infra2=digitalRead(infrared2);
  if( state_infra1==1|| state_infra2 == 1){
    return true;
  }
  else{
    return 0;
  }

}

// void ahead(){
//   setahead();
//   while (true) {
//     // Mapeo de velocidad basado en el contador de pulsos
//     //int corregir=corregir_giro();
//     SPEED_MT = map(c, 0, 35, 170/*+corregir*/,90);
//     SPEED_MT2 = map(c2, 0, 35, 170/*-corregir*/,90);
//     //set_speed(); 
//     PID();
//     if (c >= 38 ) {// Para detener el bucle cuando se alcanzan los 38 pulsos
//       digitalWrite(IN1_MT,0);
//       digitalWrite(IN2_MT,0);
//       digitalWrite(IN3_MT,0);
//       digitalWrite(IN4_MT,0);
//     }
//     if(c2>=38){
//       digitalWrite(IN5_MT,0);
//       digitalWrite(IN6_MT,0);
//       digitalWrite(IN7_MT,0);
//       digitalWrite(IN8_MT,0);
//     }
//     if(c >= 38 || c2>=38){
//       break;
//     }
//   }
//   stop(400);
//   c = 0;
//   c2=0;
// }
// void back(){
//   setback();
//   while (true) {
//     // Mapeo de velocidad basado en el contador de pulsos
//     int corregir=corregir_giro();
//     SPEED_MT = map(c, 0, 30, 120-corregir,60);
//     SPEED_MT2 = map(c2, 0, 30, 120+corregir,60); 
//     set_speed();
//     //PID();
//     if (c >= 38 ) {// Para detener el bucle cuando se alcanzan los 38 pulsos
//       break;
//     }
//   }
//   stop(0);
//   c = 0;
//   c2=0;
// }
void correccion(){
  for(int i=1; i>0; i++){ 
    Serial.println("corrijiendo");
    loop_mpu();
    getAngulo();
    if(orientacion!=0){ 
      if(angulo>orientacion){
        SPEED_MT=120;
        SPEED_MT2=120;   
        set_speed(); 
        setleft();
      }
      else if(angulo<orientacion){
      SPEED_MT=120;
      SPEED_MT2=120;   
      set_speed(); 
      setright();
      }
    if(angulo<orientacion+1 && angulo>orientacion-1){
      Serial.println("................");
      break;
    }
  }
    else{
      if(z_rotation>orientacion){
          SPEED_MT=120;
          SPEED_MT2=120;   
          set_speed(); 
          setleft();
        }
      else if(z_rotation<orientacion){
      SPEED_MT=120;
      SPEED_MT2=120;   
      set_speed(); 
      setright();
      }
      if(z_rotation<orientacion+1 && z_rotation>orientacion-1 && i>10){
        Serial.println(z_rotation);
        Serial.println("................");
        break;
      }
    }
  }
}
void ahead(){
  while (true) {
    setahead();
    // Mapeo de velocidad basado en el contador de pulsos
    //int corregir=corregir_giro();
    // SPEED_MT=140;
    // SPEED_MT2=140;
    SPEED_MT = map(c, 0, 35, 140/*+corregir*/,100);
    SPEED_MT2 = map(c2, 0, 35, 140/*-corregir*/,100);
    // set_speed(); 
    //  PID();
    if(c >= 38 || c2>=38){     
      break;
    }
    // bool linea=lineaAbajo();
    // if(linea==true){
    //   break;
    // }
  }
  stop(200);
  SPEED_MT=140;
  SPEED_MT2=140;
  set_speed(); 
  c = 0;
  c2=0;
}
void back(int pul){
  setback();
  c=0;
  c2=0;
  while (true) {
    // Mapeo de velocidad basado en el contador de pulsos
    //int corregir=corregir_giro();
    Serial.println("holaaaaaaaaaaaaaaaaaaaaaaa");
    SPEED_MT = map(c, 0, 35, 140/*-corregir*/,100);
    SPEED_MT2 = map(c2, 0, 35, 140/*corregir*/,100); 
    set_speed();
    //PID();
    if (c >= pul ) {// Para detener el bucle cuando se alcanzan los 38 pulsos
      break;
    }
  }
  stop(200);
  SPEED_MT=140;
  SPEED_MT2=140;
  set_speed(); 
  c = 0;
  c2=0;
}
// void right(){
//     SPEED_MT2=150;
//     SPEED_MT = SPEED_MT2;
//     set_speed();
//     if(orientacion==0){
//       for(int i=1;i>0;i++){ 
//         getAngulo();  
//         Serial.println(angulo);
//         // SPEED_MT = map(z_rotation, 0, (90-error_giro), 130,70);
//         setright();
//         if( angulo < 90 -error_giro && i>10){ 
//           break;
//         }
//       }
//     stop(100);
//     orientacion=90;
//     correccion();
//   }
//   else if(orientacion==90){
//     for(int i = 1; i>0; i++){
//       getAngulo();
//       Serial.println(angulo);
//       // SPEED_MT = map(z_rotation, 90, (180-error_giro), 130,70);
//       setright();
    
//       if( angulo<=180-error_giro && i>10){ 
//         break;
//       }
//     }
//     stop(100);
//     orientacion=180;
//     correccion();    
//   }
//   else if(orientacion==180){
//     for(int i =1; i>0; i++){
//       getAngulo();
//       Serial.println(angulo);
//       // SPEED_MT = map(z_rotation, -180, (-90-error_giro), 130,70);
//       setright();
    
//       if(angulo<270-error_giro && i>10){ 
//         break;
//       }
//     }
//     stop(100);
//     orientacion=270;  
//     correccion();  
//   }
//   else if(orientacion==270){
//     for(int i= 1; i>0; i++){
//       getAngulo();
//       Serial.println(angulo);
//       // SPEED_MT = map(z_rotation, -90,(0-error_giro) , 130,70);
//       // SPEED_MT2 = SPEED_MT;
//       // set_speed();
//       setright();
//       if(angulo<360-error_giro && i>10){ 
//           break;
//       }
//     }
//     c=0;
//     c2=0;
//     stop(100);
//     orientacion=0;
//     correccion();
//     c=0;
//     c2=0;    
//   }  
// }
// --------------------------------------------------------
void right(){
    SPEED_MT2=140;
    SPEED_MT2 = SPEED_MT;
    set_speed();
    if(orientacion==0){
    while(angulo<90-error_giro){
      getAngulo();  
      Serial.println(z_rotation);
      // SPEED_MT = map(z_rotation, 0, (90-error_giro), 130,70);
      setright();
    }
    stop(100);
    orientacion=90;
    correccion();
  }
  else if(orientacion==90){
    while(angulo<180-error_giro){
      getAngulo();
      Serial.println(angulo);
      // SPEED_MT = map(z_rotation, 90, (180-error_giro), 130,70);
      setright();
    }
    stop(100);
    orientacion=180; 
    correccion();   
  }
  else if(orientacion==180){
    while(angulo<270-error_giro){
      getAngulo();
      Serial.println(z_rotation);
      // SPEED_MT = map(z_rotation, -180, (-90-error_giro), 130,70);
      setright();
    }
    stop(100);
    orientacion=-90;    
  }
  else if(orientacion==-90){
    while(angulo<360-error_giro){
      getAngulo();
      Serial.println(z_rotation);
      // SPEED_MT = map(z_rotation, -90,(0-error_giro) , 130,70);
      // SPEED_MT2 = SPEED_MT;
      // set_speed();
      setright();
    }
    c=0;
    c2=0;
    stop(400);
    orientacion=0;
    c=0;
    c2=0;    
  }
}  
  // setright();
  // while (true) {
  //   loop_mpu();
  //   // Mapeo de velocidad basado giroscopio
  //   if(orientacion==0){ 
  //     SPEED_MT = map(z_rotation, 0, (90-error_giro), 130,70);
  //     if (z_rotation >= 90-error_giro) {
  //       orientacion=90;
  //       break;
  //     }
  //   }
  //   else if(orientacion==90){
  //     SPEED_MT = map(z_rotation, 90, (180-error_giro), 130,70);
  //     if (z_rotation >= 180-error_giro) {
  //       orientacion=180;
  //       break; 
  //     }
  //   }   
  //   else if(orientacion==180){
  //     if(z_rotation>-50){
  //       SPEED_MT==130;
  //     }
  //     else{
  //     SPEED_MT = map(z_rotation, -180, (-90-error_giro), 130,70);
  //     }
  //     if (z_rotation >= -90-error_giro) {
  //       orientacion=-90;
  //       break; 
  //     }
  //   }  
  //   else if(orientacion==-90){
  //     SPEED_MT = map(z_rotation, -90,(0-error_giro) , 130,70);
  //     if (z_rotation >= 0-error_giro) {
  //       orientacion=0;
  //       break; 
  //     }
  //   }   
  //   SPEED_MT2 = SPEED_MT;
  //   set_speed();
  //   Serial.println(z_rotation);
  // }
  // stop(0);
  // c = 0;
  // c2=0;
  //}
// void left(){
//   setleft();
//   while (true) {
//     loop_mpu();
//     // Mapeo de velocidad basado giroscopio
//     if(orientacion==0){ 
//       SPEED_MT = map(z_rotation, 0, (-90+error_giro), 140,80);
//       if (z_rotation <= -90+error_giro) {
//         orientacion=-90;
//         break;
//       }
//     }
//     else if(orientacion==-90){ 
//       SPEED_MT = map(z_rotation, -90, (-180+error_giro), 140,80);
//       if (z_rotation <= -180+error_giro) {
//         orientacion=180;
//         break;
//       }
//     } 
//     else if(orientacion==180){ 
//       if(z_rotation<50){
//         SPEED_MT=140;
//       }
//       else{
//         SPEED_MT = map(z_rotation, 180, (90+error_giro), 140,80);
//         if (z_rotation <= 90+error_giro) {
//         orientacion=90;
//         break;
//       }
//       }
//     }  
//     else if(orientacion==90){ 
//       SPEED_MT = map(z_rotation, 90, (0+error_giro), 140,80);
//       if (z_rotation <= 0+error_giro) {
//         orientacion=0;
//         break;
//       }
//     }  
//     SPEED_MT2 = SPEED_MT;
//     PID();
//     set_speed();
//     Serial.println(z_rotation);
//   }
//   stop(0);
//   c = 0;
//   c2=0;
// }
void PID(){
//MOTORES DERECHA
  input = (cplus * 15); // Convertir pulsos a RPM 
  cplus = 0; // Reinicia el contador para la siguiente medición
  setpoint=SPEED_MT;
  myPID.Compute();
  analogWrite(ENA_MT1, output); // Establece la velocidad del motor
  analogWrite(ENA_MT3, output);
//MOTORES IZQUIERDA
  input = (cplus2 * 15); // Convertir pulsos a RPM 
  cplus2 = 0; // Reinicia el contador para la siguiente medición
  setpoint=SPEED_MT2;
  myPID.Compute();
  analogWrite(ENA_MT2, output); // Establece la velocidad del motor
  analogWrite(ENA_MT4, output);
  delay(100);
}

void setahead(){//avance adelnate, la variable x indica cuantas unidades desea que avance
  digitalWrite(IN1_MT,0);
  digitalWrite(IN2_MT,1);
  digitalWrite(IN3_MT,0);
  digitalWrite(IN4_MT,1);
  digitalWrite(IN5_MT,0);
  digitalWrite(IN6_MT,1);
  digitalWrite(IN7_MT,0);
  digitalWrite(IN8_MT,1);
}

void setback(){//retroceso
  digitalWrite(IN1_MT,1);
  digitalWrite(IN2_MT,0);
  digitalWrite(IN3_MT,0);
  digitalWrite(IN4_MT,1);
  digitalWrite(IN5_MT,1);
  digitalWrite(IN6_MT,0);
  digitalWrite(IN7_MT,1);
  digitalWrite(IN8_MT,0);
}
void stop(int x){
  digitalWrite(IN1_MT,0);
  digitalWrite(IN2_MT,0);
  digitalWrite(IN3_MT,0);
  digitalWrite(IN4_MT,0);
  digitalWrite(IN5_MT,0);
  digitalWrite(IN6_MT,0);
  digitalWrite(IN7_MT,0);
  digitalWrite(IN8_MT,0);
  delay(x);
}
void setright(){//setear orientacion de motores
  digitalWrite(IN1_MT,1);
  digitalWrite(IN2_MT,0);//GIRAR SOBRE EL EJE
  digitalWrite(IN3_MT,0);
  digitalWrite(IN4_MT,1);
  digitalWrite(IN5_MT,1);
  digitalWrite(IN6_MT,0);
  digitalWrite(IN7_MT,0);
  digitalWrite(IN8_MT,1);
}
void setleft(){//setear orientacion de motores
  digitalWrite(IN1_MT,0);
  digitalWrite(IN2_MT,1);
  digitalWrite(IN3_MT,1);
  digitalWrite(IN4_MT,0);
  digitalWrite(IN5_MT,0);
  digitalWrite(IN6_MT,1);
  digitalWrite(IN7_MT,1);
  digitalWrite(IN8_MT,0);
}
void set_speed(){
  analogWrite(ENA_MT1,SPEED_MT);
  analogWrite(ENA_MT2,SPEED_MT2);
  analogWrite(ENA_MT3,SPEED_MT);
  analogWrite(ENA_MT4,SPEED_MT2);
}
// void right(){//girar a la derecha
//   analogWrite(ENA_MT1,SPEED_MT-(SPEED_MT*0.2));
//   analogWrite(ENA_MT2,SPEED_MT2-(SPEED_MT2*0.2));
//   analogWrite(ENA_MT3,SPEED_MT-(SPEED_MT*0.2));
//   analogWrite(ENA_MT4,SPEED_MT2-(SPEED_MT2*0.2));
//   if(orientacion==0){
//     while(z_rotation<(90-error_giro)){
//       loop_mpu(); 
//       Serial.println(z_rotation); 
//       setright();
//     }
//     stop(0);
//     orientacion=90;
//   }
//   else if(orientacion==90){
//     while(z_rotation<=179.5-error_giro){
//       loop_mpu();
//       Serial.println(z_rotation); 
//       setright();
//     }
//     stop(0);
//     orientacion=180;    
//   }
//   else if(orientacion==180){
//     while(z_rotation>-90.3 || z_rotation<-89.8-error_giro){
//       loop_mpu();
//       Serial.println(z_rotation); 
//       setright();
//     }
//     stop(0);
//     orientacion=-90;    
//   }
//   else if(orientacion==-90){
//     while(z_rotation<0-error_giro){
//       loop_mpu();
//       Serial.println(z_rotation); 
//       setright();
//     }
//     stop(0);
//     orientacion=0;    
//   }  
//   analogWrite(ENA_MT1,SPEED_MT);
//   analogWrite(ENA_MT2,SPEED_MT2);
//   analogWrite(ENA_MT3,SPEED_MT);
//   analogWrite(ENA_MT4,SPEED_MT2);
// }
// void left(){//girar a la izquierda
//   analogWrite(ENA_MT1,SPEED_MT-(SPEED_MT*0.2));
//   analogWrite(ENA_MT2,SPEED_MT2-(SPEED_MT2*0.2));
//   analogWrite(ENA_MT3,SPEED_MT-(SPEED_MT*0.2));
//   analogWrite(ENA_MT4,SPEED_MT2-(SPEED_MT2*0.2));
//   if(orientacion==0){
//     while(z_rotation>-90+error_giro){
//       loop_mpu();
//       Serial.println(z_rotation); 
//       setleft();
//     }
//     stop(0);
//     orientacion=-90;
//   }
//   else if(orientacion==-90){
//     while(z_rotation>=-179.5+error_giro){
//       loop_mpu();
//       Serial.println(z_rotation); 
//       setleft();
//     }
//     stop(0);
//     orientacion=180;    
//   }
//   else if(orientacion==180){
//     while(z_rotation<89.7 || z_rotation>90.2+error_giro){
//       loop_mpu();
//       Serial.println(z_rotation); 
//       setleft();
//     }
//     stop(0);
//     orientacion=90;    
//   }
//   else if(orientacion==90){
//     while(z_rotation>0+error_giro){
//       loop_mpu();
//       Serial.println(z_rotation); 
//       setleft();
//     }
//     stop(0);
//     orientacion=0;    
//   }  
//   analogWrite(ENA_MT1,SPEED_MT);
//   analogWrite(ENA_MT2,SPEED_MT2);
//   analogWrite(ENA_MT3,SPEED_MT);
//   analogWrite(ENA_MT4,SPEED_MT2);
// }
void distancia(){
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  // Calculamos la distancia (en cm)
  distance = (duration * 0.034) / 2; 
  Serial.println(distance);
  delay(100);
  //return distance;
}
int getcolor(){//devuelve el color
// 1:morado
// 2:rosa
// 3:amarillo
// 4:negro
  digitalWrite(s2,0);
  digitalWrite(s3,0);
  int red=pulseIn(outTCS,0);
  delay(20);
  
  digitalWrite(s2,1);
  digitalWrite(s3,1);
  int green=pulseIn(outTCS,0);
  delay(20);

  digitalWrite(s2,0);
  digitalWrite(s3,1);
  int blue=pulseIn(outTCS,0);
  delay(20);

if(green>red && green>blue && red>blue && blue<200){
  // Serial.println("Color MORADO");
  return 1;
}

else if(green>red && green>blue && blue>red){
  // Serial.println("Color ROSA");
  return 2;
}
else if(blue>green && blue>red && green>red && red<200 ){
  // Serial.println("Color AMARILLO");
  return 3;
}
else if(green>200 && blue>200 && red>200){
  // Serial.println("Color NEGRO");
    return 4;
}
// else if(green<100 && blue<100 && red<100 && (green-red)<20 && (green-blue)<20){
//   Serial.println("Color Blanco");
// }

//else{
    //Serial.println("Color no encontrado");
//}
}

void setColors(){//devuelve valores RGB
  digitalWrite(s2,0);
  digitalWrite(s3,0);
  int red=pulseIn(outTCS,1);
  delay(200);
  
  digitalWrite(s2,1);
  digitalWrite(s3,1);
  int green=pulseIn(outTCS,1);
  delay(200);

  digitalWrite(s2,0);
  digitalWrite(s3,1);
  int blue=pulseIn(outTCS,1);
  delay(200);

Serial.print("Red:");
Serial.print(red);
Serial.print(" Green:");
Serial.print(green);
Serial.print(" Blue:");
Serial.println(blue);
}

//////////////7funciones de MPU6050/////////////////////
void loop_mpu(){
  digitalWrite(8, !digitalRead(8));
  digitalWrite(9, !digitalRead(9));
  mpu.dmp_read_fifo(0);
  z_rotation=xyz[0];
}
void getAngulo(){
  loop_mpu();
  angulo = 0; 
  if(z_rotation <0 ){
    angulo = 180 +  (180+z_rotation);
  }
  else{
    angulo = z_rotation;
  }
}

void PrintValues(int32_t *quat, uint16_t SpamDelay = 100) {
  uint8_t WhoAmI;
  Quaternion q;
  VectorFloat gravity;
  
  spamtimer(SpamDelay) {
    mpu.WHO_AM_I_READ_WHOAMI(&WhoAmI);
    mpu.GetQuaternion(&q, quat);
    mpu.GetGravity(&gravity, &q);
    mpu.GetYawPitchRoll(ypr, &q, &gravity);
    mpu.ConvertToDegrees(ypr, xyz);

  }
}

void ChartValues(int32_t *quat, uint16_t SpamDelay = 100) {
  Quaternion q;
  VectorFloat gravity;
  float ypr[3] = { 0, 0, 0 };
  float xyz[3] = { 0, 0, 0 };
  spamtimer(SpamDelay) {
    mpu.GetQuaternion(&q, quat);
    mpu.GetGravity(&gravity, &q);
    mpu.GetYawPitchRoll(ypr, &q, &gravity);
    mpu.ConvertToDegrees(ypr, xyz);
  }
}

void PrintAllValues(int16_t *gyro, int16_t *accel, int32_t *quat, uint16_t SpamDelay = 100) {
  Quaternion q;
  VectorFloat gravity;
  float ypr[3] = { 0, 0, 0 };
  float xyz[3] = { 0, 0, 0 };
  spamtimer(SpamDelay) {
    mpu.GetQuaternion(&q, quat);
    mpu.GetGravity(&gravity, &q);
    mpu.GetYawPitchRoll(ypr, &q, &gravity);
    mpu.ConvertToDegrees(ypr, xyz);
  }
}

void ChartAllValues(int16_t *gyro, int16_t *accel, int32_t *quat, uint16_t SpamDelay = 100) {
  Quaternion q;
  VectorFloat gravity;
  float ypr[3] = { 0, 0, 0 };
  float xyz[3] = { 0, 0, 0 };
  spamtimer(SpamDelay) {
    mpu.GetQuaternion(&q, quat);
    mpu.GetGravity(&gravity, &q);
    mpu.GetYawPitchRoll(ypr, &q, &gravity);
    mpu.ConvertToDegrees(ypr, xyz);
  }
}

void PrintQuaternion(int32_t *quat, uint16_t SpamDelay = 100) {
  Quaternion q;
  spamtimer(SpamDelay) {
    mpu.GetQuaternion(&q, quat);
  }
}

void PrintEuler(int32_t *quat, uint16_t SpamDelay = 100) {
  Quaternion q;
  float euler[3];         
  float eulerDEG[3];         
  spamtimer(SpamDelay) {
    mpu.GetQuaternion(&q, quat);
    mpu.GetEuler(euler, &q);
    mpu.ConvertToDegrees(euler, eulerDEG);
  }
}

void PrintRealAccel(int16_t *accel, int32_t *quat, uint16_t SpamDelay = 100) {
  Quaternion q;
  VectorFloat gravity;
  VectorInt16 aa, aaReal;
  spamtimer(SpamDelay) {
    mpu.GetQuaternion(&q, quat);
    mpu.GetGravity(&gravity, &q);
    mpu.SetAccel(&aa, accel);
    mpu.GetLinearAccel(&aaReal, &aa, &gravity);
  }
}


void PrintWorldAccel(int16_t *accel, int32_t *quat, uint16_t SpamDelay = 100) {
  Quaternion q;
  VectorFloat gravity;
  VectorInt16 aa, aaReal, aaWorld;
  spamtimer(SpamDelay) {
    mpu.GetQuaternion(&q, quat);
    mpu.GetGravity(&gravity, &q);
    mpu.SetAccel(&aa, accel);
    mpu.GetLinearAccel(&aaReal, &aa, &gravity);
    mpu.GetLinearAccelInWorld(&aaWorld, &aaReal, &q);
  }
}

void print_Values (int16_t *gyro, int16_t *accel, int32_t *quat) {
  uint8_t Spam_Delay = 10; // Built in Blink without delay timer preventing Serial.print SPAM
  PrintValues(quat, Spam_Delay);
}
void inicializarMPU6050(){
  pinMode(7,OUTPUT);
  Serial.begin(115200);
  Serial.println(F("Start:"));
  mpu.begin();
  mpu.SetAddress(MPU6050_DEFAULT_ADDRESS);
  mpu.Set_DMP_Output_Rate_Hz(10);           // Set the DMP output rate from 200Hz to 5 Minutes.
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(8, HIGH);
  digitalWrite(9, LOW);
  mpu.CalibrateMPU().load_DMP_Image();// Does it all for you with Calibration
  digitalWrite(8, LOW);
  digitalWrite(9, HIGH);
  mpu.CalibrateMPU().Enable_Reload_of_DMP(Three_Axis_Quaternions).load_DMP_Image();// Does it all for you with Calibration
  mpu.on_FIFO(print_Values);
}
// ZONA C !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void girar(int directions[4][2]){
  int tempx = directions[0][0];
  int tempy = directions[0][1];
  for (int i = 0; i< 3; i++){
      directions[i][0] = directions[i+1][0];
      directions[i][1] = directions[i+1][1];
  }
  directions[3][0] = tempx;
  directions[3][1] = tempy;
  right();
  c=0;
  c2=0;
}
//arriba, izquierda, abajo, derecha
void search(bool visited[2][2], int x, int y, int directions[4][2], int backstep[2][2], int& cnt, bool& pathFound){
    visited[x][y] = true;
    if(pathFound == false){
        cnt++;
        backstep[x][y] = cnt;
    }
    if(x == 1 && y == 1){
      
      pathFound = true; 
    }
    for (int i = 0; i<4; i++){
      int newX = x + directions[0][0];
      int newY = y + directions[0][1];
      // Serial.println(newY);
      // delay(100);
      if(newX >= 0 && newY >= 0 && newX<2 && newY <2){
          if(visited[newX][newY] == false){
            ahead();
            c=0;
            c2=0;
            search(visited, newX, newY, directions, backstep, cnt, pathFound);
            back(38);
          }
        }
      girar(directions);
    }
    if(pathFound == false){
      backstep[x][y] = 30;
      cnt--;
    }
}
void fuga(int cnt, int x, int y, int directions[4][2], int backstep[2][2]){
    for(int i = 1; i < cnt; i++){
        for (int j = 0; j < 4; j++){
            int newX = x + directions[0][0];
            int newY = y + directions[0][1];
            if (newX >= 0 && newY >= 0 && newX < 2 && newY <2 && backstep[newX][newY] == i+1){
                c = 0;
                c2=0;
                ahead();
                j=4;
                x = newX;
                y = newY;
            }else{
                girar(directions);
            }
        }
    }
    ahead();
}
void zonaC() {
    //adelante, derecha, atras, izquierda
    int directions[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

    bool visited[2][2] = {{false}}; 
    int backstep[2][2] = {{30}};
    int cnt = 0;
    bool pathFound = false;
    // punto inicial
    int start_x = 0, start_y = 0;
    search(visited, start_x, start_y, directions, backstep, cnt, pathFound);
    
    fuga(cnt, start_x, start_y, directions, backstep);
    
}

//Zona A !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

bool paredAdelante(){
    lineaAbajo();
}
void left(){
  for(int i = 0; i < 3; i++){
    right();
  }
}
void pelotaEncontrada(){
    right();
    right();
    back(48);
}
void zonaA(){
  bool foundPelota = false; 
  bool foundSalida = false;
  bool lineaEncontrada = true; 
  int countD= 0;
  int countL=0;
  //busquda pelota
    while (foundPelota == false && lineaEncontrada == false){
        if(paredAdelante() == true){
            right();
            if(lineaAbajo() == false && lineaEncontrada == false){
              ahead();
              left();
            }else{
              lineaEncontrada = true;
              left();
            }
            if(lineaAbajo() == false && lineaEncontrada == false){
                ahead();
                left();
            }else{
                lineaEncontrada = true;
                left();
                ahead();
                right();
            }
            countD++;
        }else{
            pelotaEncontrada();
            foundPelota = true;
        }
    }
    while (foundPelota == false){
        if(paredAdelante() == true){
            left();
            ahead();
            right();
            ahead();
            right();
            countL++;
        }else{
            pelotaEncontrada();
        } 
    }
    //fuga 
    int pos = countD-countL;
    ahead();
    if(lineaEncontrada){
        if(abs(pos)== 2){
            ahead();
        }
        else if(pos == -3){
            left();
            ahead();
            left();
            ahead();
            right();
            ahead();
        }
        else if(pos == -1 || pos == 3){
            right();
            ahead();
            right();
            ahead();
            left();
            ahead(); 
        }
        else{
            //pos == 0;
            right();
            ahead();
            right();
            ahead();
            ahead();
            right();
            ahead();
            left();
            ahead();
        }
    }else{
        //peor caso
        //pos == 1 -> true
        // or pos == 0
        int it = 0;
        while (lineaEncontrada == false && foundSalida == false && it<2){
            left();
            if(lineaAbajo() == false && lineaEncontrada == false){
                ahead();
                left();
            }else{
                lineaEncontrada = true;
                right();
            }
            if(lineaAbajo() == false && lineaEncontrada == false){
                ahead();
                left();
            }else{
                lineaEncontrada = true;
                left();
                ahead();
                left();
            }
            if(paredAdelante() == true){
                it++;   
            }else{
                ahead();
                if(getcolor() == 5){ // "rojo
                    foundSalida = true;
                }
            }
            right();
        }
        if (foundSalida == false){
          //tanto 0 como 1 pueden hacer esto
            right();
            ahead();
            right();
            ahead();
            ahead();
            right();
            ahead();
            left();
            if(paredAdelante() == true){
                right();
                ahead();
                right();
                ahead();
                left();
            }
            if(paredAdelante() == false){
                ahead();
            }
        }
    }
}
