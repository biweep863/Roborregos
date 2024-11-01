// ZONA B !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// 0 detectar negro, 1 detectar color
//cambiar color en el caso de que sea diferente número para blanco y negro
// 5 igual a blanco, 4 a negro
void ava(){
    digitalWrite(IN1_MT,0);
    digitalWrite(IN2_MT,0);
    digitalWrite(IN3_MT,0);
    digitalWrite(IN4_MT,0);
    digitalWrite(IN5_MT,0);
    digitalWrite(IN6_MT,1);
    digitalWrite(IN7_MT,0);
    digitalWrite(IN8_MT,1);
}
void virarD(){
    digitalWrite(IN1_MT,0);
    digitalWrite(IN2_MT,0);//GIRAR SOBRE EL EJE
    digitalWrite(IN3_MT,0);
    digitalWrite(IN4_MT,0);
    digitalWrite(IN5_MT,1);
    digitalWrite(IN6_MT,0);
    digitalWrite(IN7_MT,0);
    digitalWrite(IN8_MT,1);
}
void virarI(){
    igitalWrite(IN1_MT,0);
    digitalWrite(IN2_MT,0);//GIRAR SOBRE EL EJE
    digitalWrite(IN3_MT,0);
    digitalWrite(IN4_MT,0);
    digitalWrite(IN5_MT,0);
    digitalWrite(IN6_MT,1);
    digitalWrite(IN7_MT,1);
    digitalWrite(IN8_MT,0);
}


void path(){
    // 0 no linea - 1 linea
    int infra1=digitalRead(infrared1);
    int infra2=digitalRead(infrared2);
    //0 blanco - 1 negro
    int color = blackWhite();
    Serial.print(infra2);
    Serial.print(",");
    Serial.print(color);
    Serial.print(",");
    Serial.println(infra1);
     //básico
    if (infra1 == 0 && color == 1 && infra2 == 0){
        ava();
    }//avanza V
    else if(infra1 == 1 && color == 0 && infra2 == 1){
        /*
        bool found = false;
        virarI();
        while (!found){ //sacar un rango de pasos para llegar al final
            virarI();
            infra1=digitalRead(infrared1);
            color = blackWhite();
            if(infra1 == 0 && color == 4){
                break;
            }
        }
        */
    }// Línea en la derecha
    else if(infra1 == 0 && color == 1 && infra2 == 1){
        while(infra2 == 1){
            virarI();
            infra2=digitalRead(infrared2);
        }
    }// Línea en la izquierda PID
    else if(infra1 == 1 && color == 1 && infra2 == 0){
        while(infra1 == 1){
            virarD();
            infra1=digitalRead(infrared1);
        }
    } // Línea Punteada, Blanco los tres
    else if(infra1 == 0 && color == 0 && infra2 == 0){
        int count = 0;
        bool linea = false; 
        while (count < 100 && linea == false){
            ava();
            color = blackWhite();
            if(color == 0){
                linea = true; 
            }
            count++; 
        }
    }// peor de los casos
    else if (infra1 == 1 && color == 0 && infra2 == 1){
        // tipos de patrones
        // tipo 1: linea perpendicular con línea consecuente en el frente.
        ava();
        /*
        for(int i = 0; i < 100; i++){
          ava();
        }
        color = getcolor();
        if(color != 4){

        }
        */
        // tipo 2: circulo o cuadrado con el centro despejado sin línea, recorrer el contorno
        
    }// Error, 001 and 100 
    else if (infra1 == 0 && color = 0 infra2 == 1){
        virarD();
    }
    else if (infra1 == 1 && color == 0 infra2 == 0){
        virarI();
    }
    // else{
    //     cout << "error";
    // }
}
void zonaB(){
    SPEED_MT=100 ;
    SPEED_MT2=SPEED_MT;
    set_speed();
    int infra1=digitalRead(infrared1);
    int infra2=digitalRead(infrared2);
    Serial.print(infra2);
    Serial.print(",");
    Serial.println(infra1);
    if(infra1==0 && infra2==0){
        digitalWrite(IN1_MT,0);
        digitalWrite(IN2_MT,0);
        digitalWrite(IN3_MT,0);
        digitalWrite(IN4_MT,0);
        digitalWrite(IN5_MT,0);
        digitalWrite(IN6_MT,1);
        digitalWrite(IN7_MT,0);
        digitalWrite(IN8_MT,1);
    }
    else if(infra1==0 && infra2==1){
    //   setleft();
        digitalWrite(IN1_MT,0);
        digitalWrite(IN2_MT,0);//GIRAR SOBRE EL EJE
        digitalWrite(IN3_MT,0);
        digitalWrite(IN4_MT,0);
        digitalWrite(IN5_MT,0);
        digitalWrite(IN6_MT,1);
        digitalWrite(IN7_MT,1);
        digitalWrite(IN8_MT,0);
    }
  else if(infra1==1 && infra2==0){
    // setright();
    digitalWrite(IN1_MT,0);
    digitalWrite(IN2_MT,0);//GIRAR SOBRE EL EJE
    digitalWrite(IN3_MT,0);
    digitalWrite(IN4_MT,0);
    digitalWrite(IN5_MT,1);
    digitalWrite(IN6_MT,0);
    digitalWrite(IN7_MT,0);
    digitalWrite(IN8_MT,1);
    
  }
  else if(infra1==1 && infra2==1){
    stop(0);
  }
}
