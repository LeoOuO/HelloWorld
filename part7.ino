
# include <Wire.h>
# include <SoftwareSerial.h>

//12v輸入

SoftwareSerial BT(11, 10); //HC-05 PIN, RX10,TX11

// constant
const int iend = 60;    //end command
const int speed_max = 100;  //speed max
const int speed_min = 40;   //speed_min
const int joystick_max = 50;
const int wheel1speedPIN = 3, wheel2speedPIN = 5, wheel3speedPIN = 6, wheel4speedPIN = 9; //controll motor speed
const int data = 2, big = 4, push = 7;

//variable
int cmmd[10];   //command
int insize;   //detect Bluetooth available
float vx, vy, d ;   //the data from phone, vx vx distance
int i = 0;

float v2, v1, v3, v4; // speed of four wheel
int arnd = 0; // 0: do not turn around; 1: turn  clockwise(順時針); -1: turn counterclockwise(逆時針)

int three_to_eight[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //3 to 8 資料

//function
void Stop();
int convert(int n);   //convert the value arduino receive
void get_value();   // get the value from phone
void translate();   //translate the value if the speed is lower than limit

void wheel(int wheel_num, int wheel_direction , int wheelspeed);
void  putsomething(int put_way);
void HC595();   //controll all

void move(int v);

void setup() {

  Serial.begin(9600);
  BT.begin(9600);
  pinMode(data, OUTPUT);
  pinMode(big, OUTPUT);
  pinMode(push, OUTPUT);
  pinMode(wheel1speedPIN, OUTPUT);
  pinMode(wheel2speedPIN, OUTPUT);
  pinMode(wheel3speedPIN, OUTPUT);
  pinMode(wheel4speedPIN, OUTPUT);

}


void loop() {

  get_value();
  wheel(1, (v1 / abs(v1)), abs(v1));
  wheel(2, (v2 / abs(v2)), abs(v2));
  wheel(3, (v3 / abs(v3)), abs(v3));
  wheel(4, (v4 / abs(v4)), abs(v4));



}


//=====================
void Stop() {
  vx = vy = arnd = 0;
}

int convert(int n)
{
  if (n > 50)
    return (n - 256);
  return (n);
}

void get_value()
{

  // get the bluetooth texts
  while (((insize = (BT.available())) > 0 ) && (cmmd[i] = (BT.read())) != iend)
  {
    i ++;
  }

  //One command has done
  if ((cmmd[i] ) == iend)
  {
    BT.read();  // clear 0 after list
    i = 0;

    // discriminate command
    switch (cmmd[0])    //first letter is S or J or T
    {
      case 'S':
        Stop();
        break;
      case 'J':   //J
        if (cmmd[1] == 'X')
        {
          vx = convert(cmmd[2]);
        }
        if (cmmd[3] == 'Y')
        {
          vy = convert(cmmd[4]);
        }
        if (cmmd[5] == 'D')
        {
          d = cmmd[6];
        }

      case 'T':    //T
        switch (cmmd[1])
        {
          case 'R':  //TR
            arnd = 1;


            break;
          case 'L':  //TL
            arnd = -1;
            break;
        }
        break;
    }
  }

  // change the vlue for machine
  if (arnd == 0) // don't need turn around
  {
    v1 = v3  = speed_max * ((vx + vy) / 2 / joystick_max );
    v2 = v4 = speed_max * ((vy - vx) / 2 / joystick_max );
    translate();

  }

  else if (arnd == 1)  //clockwise(順時針)
  {
    v1 = -v3 = = -v2 = v4 = speed_max / 2;
    // Serial.println("TR");

  }

  else if (arnd == -1) //counterclockwise(逆時針)
  {
    v1 = -v3 = = -v2 = v4 = -speed_max / 2;
    // Serial.println("TL");

  }

}

void translate() {
  if (abs(v1) < speed_min && v1 != 0) {

    v2 = v4 = speed_min * (v2 / v1) * (abs(v2 * v1) / int(v2 * v1)) * (abs(v2) / int(v2));
                          //比例          //抵銷比例正負                   //還原本來正負
    v1 = v3 = speed_min * (abs(v1) / int(v1));
                           //還原本來正負
  }

  if (abs(v2) < speed_min && v2 != 0) {
    v1 = v3 = speed_min * (v1 / v2) * (abs(v2 * v1) / int(v2 * v1)) * (abs(v1) / int(v1));
                          //比例          //抵銷比例正負                   //還原本來正負    
    v2 = v4 = speed_min * (abs(v2) / int(v2));
                           //還原本來正負
  }
  v1 = v3 = int(v1);
  v2 = v4 = int(v2);
}

/*
  IN1 IN2 狀態
  1    0  正轉
  0    1  反轉
  0    0  停止
*/

//12v輸入



void wheel(int aa, int bb , int wheelspeed) {
  //第一個馬達
  if (aa == 1) {
    if (bb == 1) {
      three_to_eight[0] = 1;
      three_to_eight[1] = 0;
      analogWrite(wheel1speedPIN, map(wheelspeed, 0, 100, 0, 255));
    }
    if (bb == -1) {
      three_to_eight[0] = 0;
      three_to_eight[1] = 1;
      analogWrite(wheel1speedPIN, map(wheelspeed, 0, 100, 0, 255));
    }
    if (bb == 0) {
      three_to_eight[0] = 0;
      three_to_eight[1] = 0;
      analogWrite(wheel1speedPIN, 0);
    }
  }
  //第二個馬達
  if (aa == 2) {
    if (bb == 1) {
      three_to_eight[2] = 1;
      three_to_eight[3] = 0;
      analogWrite(wheel2speedPIN, map(wheelspeed, 0, 100, 0, 255));
    }
    if (bb == -1) {
      three_to_eight[2] = 0;
      three_to_eight[3] = 1;
      analogWrite(wheel2speedPIN, map(wheelspeed, 0, 100, 0, 255));
    }
    if (bb == 0) {
      three_to_eight[2] = 0;
      three_to_eight[3] = 0;
      analogWrite(wheel1speedPIN, 0);
    }
  }
  //第三個馬達
  if (aa == 3) {
    if (bb == 1) {
      three_to_eight[4] = 1;
      three_to_eight[5] = 0;
      analogWrite(wheel3speedPIN, map(wheelspeed, 0, 100, 0, 255));
    }
    if (bb == -1) {
      three_to_eight[4] = 0;
      three_to_eight[5] = 1;
      analogWrite(wheel3speedPIN, map(wheelspeed, 0, 100, 0, 255));
    }
    if (bb == 0) {
      three_to_eight[4] = 0;
      three_to_eight[5] = 0;
      analogWrite(wheel1speedPIN, 0);
    }
  }
  //第四個馬達
  if (aa == 4) {
    if (bb == 1) {
      three_to_eight[6] = 1;
      three_to_eight[7] = 0;
      analogWrite(wheel4speedPIN, map(wheelspeed, 0, 100, 0, 255));
    }
    if (bb == -1) {
      three_to_eight[6] = 0;
      three_to_eight[7] = 1;
      analogWrite(wheel4speedPIN, map(wheelspeed, 0, 100, 0, 255));
    }
    if (bb == 0) {
      three_to_eight[6] = 0;
      three_to_eight[7] = 0;
      analogWrite(wheel1speedPIN, 0);
    }
  }
  HC595();
  // Serial.println(bb);
}

void HC595() {
  for (int iii = 7 ; iii > -1; iii--) {
    putsomething(three_to_eight[iii]);
  }
  digitalWrite(big, 1);
  digitalWrite(big, 0);
}
void  putsomething(int bbb) {
  digitalWrite(data, bbb);
  digitalWrite(push, 1);
  digitalWrite(push, 0);
}
