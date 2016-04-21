import blinkstick.*;
import controlP5.*;
import javax.swing.*;
import java.awt.Robot;
import java.awt.AWTException;
import java.awt.event.InputEvent;
import processing.serial.*;   //read sensor data from Arduino port

PrintWriter createWriter;
String[] loadStrings;

ControlP5 cp5;
Serial myPort;  // Create object from Serial class
//UDP udp;  // define the UDP object For Serial app
Textarea myTextarea;
Println console;
ControlTimer c;
Textlabel t;
GuiColorPicker cp;
BlinkStick blinkStick;
final float TO_VOLTAGE = 0.008F;
int A_Pos, B_Pos, C_Pos, D_Pos, E_Pos, pos_default_value = 2000, start_pos = 300, PosRange = 3700, speed_range = 255, speed_start = -255 ;
int A_Speed, B_Speed, C_Speed, D_Speed, E_Speed;
int mouse_wheel_state = 0, wheel_speed = 3;
//K,D1,M0  M0 == ask M1 == sel_detect  M2  == auto 
int auto_mode = 1, ask_mode = 2, self_detect_mode = 3, cool_style=1;
int rx_buffer_size=100;
String messageReceive="2,0,0", learnmessage = "leran_frome_user.txt";
byte[] rx_byte=new byte[rx_buffer_size];
int rx_byte_count=0;
int LocalPort = (int)random(200, 8080);
String LocalIp = " ";
int [] int_data = new int [4];      // store serial data;
float [] float_data = new float[3]; //store serial data;

float i = 0.001;
boolean         unlockSpeed = true, unlockPos = true, button_smooth_pressed = false, 
Start_Learn = false, flag_lock1 = true, 
flag_lock2 = true, flag_lock3 = true, 
flag_lock4 = true, flag_stop_pressed = false, 
flag_learn_pressed = false, flag_run_pressed = false, 
action_display_allowed = false, updata = true, 
tab_switch = true, IsReleased = false, 
flag_position_toggle_pressed = true, flag_speed_toggle_pressed = true, display_enable = true;
int tabHeight=20; // Extra height needed for Tabs
int windowsX    = 950;       
int windowsY    = 500;
int myColorBackground = color(0, 0, 0);
int knobValue = 100;
int get_threshold = 2;
int defalt_Speed = 200;
int current_frame_count = 0, last_frame_count = 0, dif_count = 2;

int ZeroX = 0, ZeroY = 0, offsetX = 20, offsetY = 20, vertical = 40, horizontal = 30, Temp_distance = 320;
int currentX = ZeroX + offsetX, currentY = ZeroY + offsetY, Radius = 50;
int send_sucess = 1;
int index = 0; 
int iqw = 1;
int test_i = 0;
int sendfre;
int ID;
int pos;
int speed;
int voltage;
int temp; 
int position_sample = 300;
long lasttime_a, lasttime_b, lasttime_c, lasttime_d, lasttime_e;
Knob myKnobA_Speed, myKnobA_Pos;
Knob myKnobB_Speed, myKnobB_Pos;
Knob myKnobC_Speed, myKnobC_Pos;
Knob myKnobD_Speed, myKnobD_Pos;
Knob myKnobE_Speed, myKnobE_Pos;
smart_Servo S1 = new smart_Servo(1, 0, 4090);
smart_Servo S2 = new smart_Servo(2, 0, 4090);
smart_Servo S3 = new smart_Servo(3, 0, 4090);
smart_Servo S4 = new smart_Servo(4, 0, 4090);
smart_Servo S5 = new smart_Servo(5, 0, 4090);
int updata_servo_id = 1;
int dir_flag= 0;
void setup() {
  size(1200, 500);
  //orientation(LANDSCAPE);
  //textAlign(CENTER, CENTER);
  textSize(36);
  smooth();
  noStroke();
  // frameRate(180);
  cool_style = 0;
  gui();
  cp = new GuiColorPicker( 950, 0, 250, windowsY-180, 255 ); 
  blinkStick = BlinkStick.findFirst();
  println(Serial.list()[0]); 
  loadStrings = loadStrings(learnmessage);
  createWriter = createWriter(learnmessage);
  myPort = new Serial(this, "COM2", 115200); 
  myPort.buffer(256);
  println("===========================");
  print("Robotic Serial Port:");
  println("===========================");
  println("Makeblock Robotic Arm Demo");
  println("Smart Servos Design by Makeblcok");
  println("============================");
  println("Visit www.makeblock.cc for detail.");
  //  myPort.clear();

  //frame.setTitle("5 Dof Robotic Arm"); 

  cp5.getController("Edit_Pos").setValue(1);
  cp5.getController("Edit_Speed").setValue(1);
  cp5.getController("A_Release").setValue(1);
  cp5.getController("B_Release").setValue(1);
  cp5.getController("C_Release").setValue(1);
  cp5.getController("D_Release").setValue(1);
  cp5.getController("E_Release").setValue(1);


  //frame.setTitle("5 Dof Robotic Arm"); 
  cp5.getController("Edit_Speed").setValue(1);
  cp5.getController("Edit_Pos").setValue(1);

  // delay(1000);

  initialize();
  get_current_pos_and_speed(1, self_detect_mode);
  get_current_pos_and_speed(2, self_detect_mode);
  get_current_pos_and_speed(3, self_detect_mode);
  get_current_pos_and_speed(4, self_detect_mode);
  get_current_pos_and_speed(5, self_detect_mode);
  //  S1.assignID();  
  new smart_Servo().assignID();//niming object

   S1.set_Report_Mode(SELFDETECT_MODE, GET_POSITION);
   S2.set_Report_Mode(SELFDETECT_MODE, GET_POSITION);
  delay(100);
  //S1.set_Report_Mode(SELFDETECT_MODE, GET_VOLTAGE);
  //  S1.set_Report_Mode(AUTO_MODE, GET_TEMPRATURE);
  //
  //  S2.set_Report_Mode(SELFDETECT_MODE, GET_POSITION);
  //
  //  S3.set_Report_Mode(SELFDETECT_MODE, GET_POSITION);
  //
  //  S4.set_Report_Mode(SELFDETECT_MODE, GET_POSITION);
  //
  //  S5.set_Report_Mode(SELFDETECT_MODE, GET_POSITION);
  new smart_Servo().switch_to_1M();
  //new smart_Servo().switch_to_10K();
}

void draw() {
 new smart_Servo().servo_test_command(1);
  new smart_Servo().servo_test_command(2);
 test_i = test_i+1;
 print( "Tolal send:"+test_i+" ");
  S1.print_n();
  //    S1.get_Pos();
  //  S2.get_Pos();
  while (myPort.available () > 0) {
    int inf_from_servo=0;
    byte[] inf_back = new byte[8]; // back f0 01 10 70 f7
    int lf = (byte)0xF7;
    // for(int i = 0;i<=7;i++)
    //{
    //inf_back = myPort.readBytes();
    //}  
    myPort.readBytes(inf_back);
    //myPort.readBytesUntil(lf, inf_back);

    if (inf_back != null) {
      try {
        if ((inf_back[0] == (byte)START_SYSEX) && (inf_back[2] == (byte)CTL_ERROR_CODE))
        {  
          //print(inf_back[1]);
          switch(inf_back[3])//switch error code!
          {
            case (byte)PROCESS_SUC :
            // println("success!");
            send_success = send_success + 1;
            break;
            case (byte)PROCESS_BUSY :
            send_busy = send_busy + 1 ;
            println("Servos are busy!");
            break;
            case (byte)PROCESS_ERROR:
            send_process_wrong = send_process_wrong + 1;
            println("Something Wrong While Assigns!");
            break;
            case (byte)WRONG_TYPE_OF_SERVICE:
            send_wrong_type = send_wrong_type + 1;
            println("Wrong Type of Service!");
            break;
          }
        }
        if ((inf_back[0] == (byte)START_SYSEX)  && (inf_back[2] == (byte)SERVO_TYPE))
        {

          switch(inf_back[1])
          {
          case 1:
            switch(inf_back[3]) //process get command
            {
              case (byte)GET_POSITION:   
              S1.pos_from_servo = new smart_Servo().receive_Short(inf_back );
              break;
              case (byte)GET_SPEED:      
              S1.speed_from_servo = new smart_Servo().receive_Short(inf_back );
              break;
              case (byte)GET_TEMPRATURE: 
              S1.temp_from_servo = new smart_Servo().calculate_temp(new smart_Servo().receive_Short(inf_back ));
              break;
              case (byte)GET_CURRENT:    
              S1.current_from_servo = new smart_Servo().receive_Short(inf_back );
              break;
              case (byte)GET_VOLTAGE:    
              S1.voltage_from_servo = new smart_Servo().receive_Short(inf_back )*TO_VOLTAGE;
              break;
            }
            break;
          case 2:
            switch(inf_back[3]) //process get command
            {
              case (byte)GET_POSITION:   
              S2.pos_from_servo = new smart_Servo().receive_Short(inf_back );
              break;
              case (byte)GET_SPEED:      
              S2.speed_from_servo = new smart_Servo().receive_Short(inf_back );
              break;
              case (byte)GET_TEMPRATURE: 
              S2.temp_from_servo = new smart_Servo().calculate_temp(new smart_Servo().receive_Short(inf_back ));
              break;
              case (byte)GET_CURRENT:    
              S2.current_from_servo = new smart_Servo().receive_Short(inf_back );
              break;
              case (byte)GET_VOLTAGE:    
              S2.voltage_from_servo = new smart_Servo().receive_Short(inf_back )*TO_VOLTAGE;
              break;
            }
            break;
          }
        }
      }
      catch (Exception e)
      {
        //*********************************************************
      }
    }
  }

  cp5.getController("A_Pos").setValue(S1.pos_from_servo);
  cp5.getController("B_Pos").setValue(S2.pos_from_servo);

  if (mousePressed)
  {
    S1.go_To(mouseX*4, (int)map(mouseY, 0, height, 0, 255));
    ;
  }
  //update_pos();
  updata_servo_id = updata_servo_id+1;
  //  S1.handle_receive();

  if (updata_servo_id == 6)
  {
    updata_servo_id = 1;
  }

  if (position_sample<=400) {
    dir_flag = 0;
    position_sample = 400;
  }
  if (position_sample>=3300) {
    dir_flag = 1;
    position_sample = 3300;
  }
  switch(dir_flag)
  {
  case 0:
    position_sample= position_sample+100;
    break;
  case 1:
    position_sample= position_sample-100;
    break;
  }
  if (send_sucess == 1)
  {
 S1.go_To( position_sample, 255);
     S2.go_To( position_sample, 255);
    //S4.go_To( position_sample,255);
    sendfre = sendfre + 1;
  }
  //println(sendfre+"," + iqw + ", " + sendfre/iqw);
  //  myPort.write(START_SYSEX);//assign f0 ff 10 00 f7
  //  myPort.write(0xff);
  //  myPort.write(ASSIGN_ID);
  //  myPort.write(START_ID);
  //  myPort.write(END_SYSEX);
  //println( "Temp:"+S1.get_Tempreture()+","+"Current:"+S1.get_Current()+"," +"Voltage"+S1.get_Voltage()+","+"POS:"+S1.get_Pos()+","+"Speed:"+S1.get_Speed());
  //println( S1.get_Pos());
  //F0 01 70 11 38 17 00 7f 01 00 f7

  // S1.send_Short(mouseX*4);
  //  S1.get_Speed();
  //  S1.handle_receive();
  //
  //  S2.get_Speed();
  //  S2.handle_receive();
  //
  //  S1.get_Pos();
  //  S1.handle_receive();
  //
  ////  if (millis()- lasttime_a>= 100)
  ////  {
  //    lasttime_a = millis();
  //    S1.get_Pos();
  //    S1.handle_receive();
  ////  }
  ////    if (millis()- lasttime_b>= 120)
  ////  {
  //    lasttime_b = millis();
  //    S2.get_Pos();
  //    S2.handle_receive();
  // // }
  ////    if (millis()- lasttime_c>= 140)
  ////  {
  //    lasttime_c = millis();
  //    S3.get_Pos();
  //    S3.handle_receive();
  ////  }
  ////    if (millis()- lasttime_d>= 160)
  ////  {
  //   // lasttime_d = millis();
  //    S4.get_Pos();
  //    S4.handle_receive();
  ////  }
  ////    if (millis()- lasttime_e>= 180)
  ////  {
  //    lasttime_e = millis();
  //    S5.get_Pos();
  //    S5.handle_receive();
  ////  }
  //  if (frameCount%3==0)
  //  {
  //    S2.get_Pos();
  //    S2.handle_receive();
  //    cp5.getController("B_Pos").setValue(S2.pos_from_servo);
  //  }

  // if (frameCount%3==0)



  //
  //  S1.get_Voltage();
  //  S1.handle_receive();
  //
  //  S2.get_Voltage();
  //  S2.handle_receive();

  //  S1.get_Current();
  //  S1.handle_receive();
  //
  //  S2.get_Current();
  //  S2.handle_receive();
  //  //S1.handle_receive();
  //
  //  S1.get_Tempreture();
  //  S1.handle_receive();
  //  S2.get_Pos();
  //  S2.get_Tempreture();
  //  S2.handle_receive();

  //    cp5.getController("A_Pos").setValue(S1.pos_from_servo);
  //  cp5.getController("C_Pos").setValue(S3.pos_from_servo);
  //  cp5.getController("D_Pos").setValue(S4.pos_from_servo);
  //  cp5.getController("E_Pos").setValue(S5.pos_from_servo);

  //  //cp5.getController("B_Pos").setValue(S1.current_from_servo);
  //  cp5.getController("A_temprature").setValue(S1.temp_from_servo);
  //  cp5.getController("A_Voltage").setValue(S1.voltage_from_servo);
  //  //cp5.getController("A_Pos").setValue(position);
  //  cp5.getController("A_Speed").setValue(S1.speed_from_servo);
  //  print(Math.round(S1.current_from_servo/4096*3000)+"  ");
  //  print(Math.round(S2.current_from_servo/4096*3000));
  //  println(" mA");
  //  cp5.getController("B_Pos").setValue(S2.pos_from_servo);
  //  //cp5.getController("B_Pos").setValue(S1.current_from_servo);
  //  cp5.getController("B_temprature").setValue(S2.temp_from_servo);
  //  cp5.getController("B_Voltage").setValue(S2.voltage_from_servo);
  //  //cp5.getController("A_Pos").setValue(position);
  //  cp5.getController("B_Speed").setValue(S2.speed_from_servo);
  // if(mouseX>200)
  // {
  // myPort.write(0xf0);
  // myPort.write(0x01);
  // myPort.write(0x70);
  // myPort.write(0x11);
  // myPort.write(0x38);
  // myPort.write(0x17);
  // myPort.write(0x00);
  // myPort.write(0x7f);
  // myPort.write(0x01);
  // myPort.write(0x00);
  // myPort.write(0xf7);
  // }

  // myPort.write(0xf0);
  // myPort.write(0x01);
  // myPort.write(0x70);
  // myPort.write(0x16);
  // myPort.write(0x01);
  // myPort.write(0xf7);
  // 
  // myPort.write(0xf0);
  // myPort.write(0x01);
  // myPort.write(0x70);
  // myPort.write(0x11);
  // myPort.write(0x10);
  // myPort.write(0x03);
  // myPort.write(0x00);
  // myPort.write(0x7f);
  // myPort.write(0x01);
  // myPort.write(0x00);
  // myPort.write(0xf7);
  //f0 01 70 16 01 f7
  //}
  // if(millis()%20 == 0){
  //        cp5.getController("A_Pos").setValue(S1.get_Pos());
  //
  //// updata_data_display(1, S1.get_Pos(),0, 0, 0,true);
  // }  //println(S1.get_Tempreture());
  //   //println(S1.get_Voltage()/4095F*33);;
  // if(millis()%4 == 0){
  // //updata_data_display(1, 0,0, 0, S1.get_Tempreture(),true);
  //      cp5.getController("A_Voltage").setValue(S1.get_Voltage()/4095F*33);
  //
  //    }
  //  int lf = END_SYSEX;
  //  byte[] inBuffer = new byte[8]; // back f0 01 10 70 f7
  //  if (myPort.available () > 0) {
  //    //inBuffer = myPort.readBytes();
  //    myPort.readBytesUntil(lf, inBuffer);
  //    if (inBuffer != null) {
  //      if (inBuffer[0] == (byte)START_SYSEX)
  //      {
  //        int id = inBuffer[1]&0xFF;
  //        switch (inBuffer[2])
  //        {
  //          case 0x10 : println("Now assigning ID !");break;
  //        
  //          case 0x70 : println("The devices are Servos !");break;  
  //          
  //        }
  //        
  //        println(inBuffer);
  //      }
  //    }
  //  }
  //  
  //  void process_Command(byte command)
  //  {
  //    switch (command)
  //    {
  //      case 0x22:back_Pos = readShort();break;
  //      case 0x23:back_Speed = readShort();break;
  //      case 0x25:back_Temp = readShort();break;
  //      case 0x27:back_Voltage = readShort();break;
  //      default:
  //      break;
  //    }
  //  }
  //  if(millis()%100==0)
  //  {
  // assignID();
  //  }

  switch (cool_style)
  {
  case 1: 
    cp5.getController("Ser"+1).setColorForeground(color(red, green, blue));
    set_led_color(1, red, green, blue);
    break;
  case 2: 
    cp5.getController("Ser"+2).setColorForeground(color(red, green, blue));
    set_led_color(2, red, green, blue);
    break;
  case 3: 
    cp5.getController("Ser"+3).setColorForeground(color(red, green, blue));
    set_led_color(3, red, green, blue);
    break;
  case 4: 
    cp5.getController("Ser"+4).setColorForeground(color(red, green, blue));
    set_led_color(4, red, green, blue);
    break;
  case 5: 
    cp5.getController("Ser"+5).setColorForeground(color(red, green, blue));
    set_led_color(5, red, green, blue);
    break;
  case 6: 
    for (int i = 1;i<=6;i++) {
      cp5.getController("Ser"+i).setColorForeground(color(red, green, blue));
      set_led_color(i, red, green, blue);
    }
    break;
  }

  if (button_smooth_pressed)
  {
    cp5.getController("Speed_Learn").setLock(true);//.setValue();
    //cp5.getController("Speed_execute").setLock(true);//.setValue();

    Speed_Learn = 1;
    //Smooth_speed = Speed_execute;
    if (millis()%Speed_execute==0)
    {
      for (int i = 1;i<=5;i++)
      {
        if (millis()%5==0)
        {
          // myPort.write("K"+","+"D"+i+","+"M"+2+"#");
        }
      }
    }
  }
  else
  {
    cp5.getController("Speed_Learn").setLock(false);//.setValue();
  }
  //println(mouseX+","+""+mouseY);
  background(59);
  fill(0, 100);
  rect(0, 0, 140, height);
  rect(currentX+Radius*4+offsetX*3, currentY-offsetY, 140, height);
  rect(currentX+Radius*8+offsetX*7, currentY-offsetY, 140, height);
  fill(30, 120);
  rect(0, height/2, width, height/2);
  t.setValue(c.toString());
  t.draw(this);
  t.setPosition(790, 20);
  //enable_get_current_pos_and_speed(tab_switch,get_threshold);
  //println(mouseX+" "+mouseY);
  get_command_from_user_mouse();//mouse operation to servos!
  if (flag_run_pressed) {


    load_learn_data_then_action();
  }
  pushMatrix(); 
  fill(0, 150, 150);
  PFont v = createFont("MicrosoftPhagsPa", 14); 
  PFont s = createFont("MicrosoftPhagsPa", 20); 

  textFont(v);
  //text("5 Dof Robotic Arm Demo Design By Makeblock", 40, 480);
  //text("5 Dof Robot Demo", 500, 500);
  if (int_data[1]>=3700||int_data[1]<=300)
  {
    //set_motor_released(0,int_data[0]);
  }
  fill(0, 100);
  rect(windowsX, 0, 250, height);
  cp.render();
  fill(255);
  text("Running: ", 720, 32);
  textFont(s);
  textSize(80);
  text(S1.pos_from_servo, 400, 400);
  text("Color Picker", 1010, 353);
  popMatrix();
}

////**********************************Events*************************************//
void A_Pos(int theValue) { 
  A_Pos = theValue;
}
void B_Pos(int theValue) { 
  B_Pos = theValue;
}
void C_Pos(int theValue) { 
  C_Pos = theValue;
}
void D_Pos(int theValue) { 
  D_Pos = theValue;
}
void E_Pos(int theValue) { 
  E_Pos = theValue;
}
void A_Speed(int theValue) { 
  A_Speed = theValue;
}
void B_Speed(int theValue) { 
  B_Speed = theValue;
}
void C_Speed(int theValue) { 
  C_Speed = theValue;
}
void D_Speed(int theValue) { 
  D_Speed = theValue;
}
void E_Speed(int theValue) { 
  E_Speed = theValue;
}

//void keyPressed() {
//  S1.set_Servo_Coast(0);
//}
void keyPressed() {
  if (key == CODED) {
    if (keyCode == UP) {
      S1.go_To( 3000, 255);

      S2.go_To( 400, 255);
      S3.go_To( 400, 255);

      //      S1.set_Servo_Coast(0);
      //      S2.set_Servo_Coast(0);
    } 
    else if (keyCode == DOWN) {
      S1.go_To( 400, 255);

      S2.go_To( 3000, 255);
      S3.go_To( 3000, 255);

      //      S1.set_Servo_Coast(0);
      //      S2.set_Servo_Coast(0);
    }
  } 
  else {
  }
}

//void addMouseWheelListener() {
//  frame.addMouseWheelListener(new java.awt.event.MouseWheelListener() {
//    public void mouseWheelMoved(java.awt.event.MouseWheelEvent e) {
//      cp5.setMouseWheelRotation(e.getWheelRotation()*wheel_speed);
//      mouse_wheel_state = e.getWheelRotation();
//      //println(e.getWheelRotation());
//    }
//  }
//  );
//}
void Smooth(boolean theFlag) {
  if (theFlag==true) {
    button_smooth_pressed = true;
  } 
  else {
    button_smooth_pressed = false;
  }
}
void Edit_Speed(boolean theFlag) {
  if (theFlag==true) {
    if (flag_lock1 == true) {
      unlockSpeed = true;
      cp5.getController("A_Speed").setLock(unlockSpeed);
      cp5.getController("B_Speed").setLock(unlockSpeed);    
      cp5.getController("C_Speed").setLock(unlockSpeed);    
      cp5.getController("D_Speed").setLock(unlockSpeed);    
      cp5.getController("E_Speed").setLock(unlockSpeed);    
      //println(i++);
      flag_lock1 = false;
      flag_lock2 = true;
      display_switch = true;

      //get_current_pos_and_speed(1, ask_mode);
      display_enable = true;
    }
  } 
  else {
    if (flag_lock2 == true) {
      unlockSpeed = false;
      cp5.getController("A_Speed").setLock(unlockSpeed);
      cp5.getController("B_Speed").setLock(unlockSpeed);    
      cp5.getController("C_Speed").setLock(unlockSpeed);    
      cp5.getController("D_Speed").setLock(unlockSpeed);    
      cp5.getController("E_Speed").setLock(unlockSpeed);    
      //println("loop");
      flag_lock2 =false;
      flag_lock1 = true;
      display_switch = false;

      //get_current_pos_and_speed(true, self_detect_mode);
      display_enable = false;
    }
  }
}
void Edit_Pos(boolean theFlag) {
  if (theFlag==true) {
    if (flag_lock3 == true) {
      unlockPos = true;
      cp5.getController("A_Pos").setLock(unlockPos);
      cp5.getController("B_Pos").setLock(unlockPos);    
      cp5.getController("C_Pos").setLock(unlockPos);    
      cp5.getController("D_Pos").setLock(unlockPos);    
      cp5.getController("E_Pos").setLock(unlockPos); 
      flag_lock3 = false;
      flag_lock4 = true;
      //get_current_pos_and_speed(true, auto_mode);
      display_enable = true;
      display_switch = true;
    }
  } 


  else {
    if (flag_lock4 == true) {
      unlockPos = false;
      cp5.getController("A_Pos").setLock(unlockPos);
      cp5.getController("B_Pos").setLock(unlockPos);    
      cp5.getController("C_Pos").setLock(unlockPos);    
      cp5.getController("D_Pos").setLock(unlockPos);    
      cp5.getController("E_Pos").setLock(unlockPos);   
      flag_lock4 =false;
      flag_lock3 = true;
      // get_current_pos_and_speed(true, self_detect_mode);
      display_enable = false;
      display_switch = false;
    }
  }
}
void A_Release(boolean theFlag) {
  if (theFlag==true) {
    set_motor_released(0, 1);
  }
  else
    set_motor_released(1, 1);
}

void B_Release(boolean theFlag) {
  if (theFlag==true) {
    set_motor_released(0, 2);
  }
  else
    set_motor_released(1, 2);
}

void C_Release(boolean theFlag) {
  if (theFlag==true) {
    set_motor_released(0, 3);
  }
  else
    set_motor_released(1, 3);
}

void D_Release(boolean theFlag) {
  if (theFlag==true) {
    set_motor_released(0, 4);
  }
  else
    set_motor_released(1, 4);
}

void E_Release(boolean theFlag) {
  if (theFlag==true) {
    set_motor_released(0, 5);
  }
  else
    set_motor_released(1, 5);
}
public void controlEvent(ControlEvent theEvent) {
  try {
    for (int i=1;i<=6;i++) {
      if (theEvent.getController().getName().equals("Ser"+i)) {
        set_led_color(i, red, green, blue);
        cp5.getController("Ser"+i).setColorForeground(color(red, green, blue));
        cp5.getController("Ser"+i).setColorBackground(color(red, green, blue));

        switch(i)
        {
        case 1: 
          cool_style = 1;
          break;
        case 2: 
          cool_style = 2;
          break; 
        case 3: 
          cool_style = 3;
          break; 
        case 4: 
          cool_style = 4;
          break; 
        case 5: 
          cool_style = 5;
          break; 
        case 6: 
          cool_style = 6;
          break;
        }
      }
    }

    if (theEvent.getController().getName()== "Start_Learn") {
      flag_learn_pressed = true;
      get_current_pos_and_speed(1, self_detect_mode);
      get_current_pos_and_speed(2, self_detect_mode);
      get_current_pos_and_speed(3, self_detect_mode);
      get_current_pos_and_speed(4, self_detect_mode);
      get_current_pos_and_speed(5, self_detect_mode); 
      //get_current_pos_and_speed(true, self_detect_mode);
      release_all_servos();
      // println("flag learn pressed = true");
    }
    if (theEvent.getController().getName()== "Learn_Finish") {
      flag_learn_pressed = false;
      get_current_pos_and_speed(1, self_detect_mode);
      get_current_pos_and_speed(2, self_detect_mode);
      get_current_pos_and_speed(3, self_detect_mode);
      get_current_pos_and_speed(4, self_detect_mode);
      get_current_pos_and_speed(5, self_detect_mode); 
      updata = false;
      for (int i = 1;i<=5;i++)
      {
        set_motor_released(0, i);
        //delay(50);
      }
      createWriter.flush(); // Write the remaining data
      createWriter.close(); // Finish the file
    }
    if (theEvent.getController().getName()== "run") {
      flag_run_pressed = true;
      flag_stop_pressed = false;
      //get_current_pos_and_speed(true, ask_mode);
      index = 0; 
      loadStrings = loadStrings(learnmessage);
    }
    if (theEvent.getController().getName()== "stop") {
      flag_stop_pressed = true;
      flag_run_pressed = false;
      createWriter = createWriter(learnmessage);
    }
    if (theEvent.getController().getName()== "Edit_Pos") {
      flag_position_toggle_pressed = true;
    }
    else
      flag_position_toggle_pressed = false;

    if (theEvent.getController().getName()== "Edit_Speed") {
      flag_speed_toggle_pressed = true;
    }
    else
      flag_speed_toggle_pressed = false;
  }
  catch (Exception e) {
    // println("last");
  }
}
void servo_one_update()
{
  S1.get_Pos();
  //  S1.handle_receive();

  //  S1.get_Voltage();
  //  //S1.handle_receive();
  //
  //  S1.get_Current();
  //  S1.handle_receive();
  //
  //  S1.get_Tempreture();
  //  //S1.handle_receive();
  //
  //  S1.get_Speed();
  //  S1.handle_receive();

  cp5.getController("A_Pos").setValue(S1.pos_from_servo);
  cp5.getController("A_Speed").setValue(S1.current_from_servo);
  cp5.getController("A_Voltage").setValue(S1.voltage_from_servo);
  cp5.getController("A_temprature").setValue(S1.temp_from_servo);
  // cp5.getController("A_Pos").setValue(S1.current_from_servo);
}
void servo_two_update()
{
  S2.get_Pos();
  //  S2.handle_receive();

  //  S2.get_Voltage();
  //  //S2.handle_receive();
  //
  //  S2.get_Current();
  //  S2.handle_receive();
  //
  //  S2.get_Tempreture();
  // // S2.handle_receive();
  //
  //  S2.get_Speed();
  //  S2.handle_receive();

  cp5.getController("B_Pos").setValue(S2.pos_from_servo);
  cp5.getController("B_Speed").setValue(S2.current_from_servo);
  cp5.getController("B_Voltage").setValue(S2.voltage_from_servo);
  cp5.getController("B_temprature").setValue(S2.temp_from_servo);
  //cp5.getController("A_Pos").setValue(S2.current_from_servo);
}
void servo_three_update()
{
  S3.get_Pos();
  //  S3.handle_receive();

  //  S3.get_Voltage();
  // // S3.handle_receive();
  //
  //  S3.get_Current();
  //  S3.handle_receive();
  //
  //  S3.get_Tempreture();
  //  //S3.handle_receive();
  //
  //  S3.get_Speed();
  //  S3.handle_receive();

  cp5.getController("C_Pos").setValue(S3.pos_from_servo);
  cp5.getController("C_Speed").setValue(S3.current_from_servo);
  cp5.getController("C_Voltage").setValue(S3.voltage_from_servo);
  cp5.getController("C_temprature").setValue(S3.temp_from_servo);
  //cp5.getController("A_Pos").setValue(S3.current_from_servo);
}
void servo_four_update()
{
  S4.get_Pos();
  //  S4.handle_receive();

  //  S4.get_Voltage();
  //   //S4.handle_receive();
  //
  //  S4.get_Current();
  //  S4.handle_receive();
  //
  //  S4.get_Tempreture();
  // // S4.handle_receive();
  //
  //  S4.get_Speed();
  //  S4.handle_receive();

  cp5.getController("D_Pos").setValue(S4.pos_from_servo);
  cp5.getController("D_Speed").setValue(S4.current_from_servo);
  cp5.getController("D_Voltage").setValue(S4.voltage_from_servo);
  cp5.getController("D_temprature").setValue(S4.temp_from_servo);
  //cp5.getController("A_Pos").setValue(S4.current_from_servo);
}
void servo_five_update()
{
  S5.get_Pos();
  //  S5.handle_receive();

  //  S5.get_Voltage();
  //  //S5.handle_receive();
  //
  //  S5.get_Current();
  //  S5.handle_receive();
  //
  //  S5.get_Tempreture();
  // // S5.handle_receive();
  //
  //  S5.get_Speed();
  //  S5.handle_receive();

  cp5.getController("E_Pos").setValue(S5.pos_from_servo);
  cp5.getController("E_Speed").setValue(S5.current_from_servo);
  cp5.getController("E_Voltage").setValue(S5.voltage_from_servo);
  cp5.getController("E_temprature").setValue(S5.temp_from_servo);
  //cp5.getController("E_Pos").setValue(S5.current_from_servo);
}
/*
a list of all methods available for the Knob Controller
 use ControlP5.printPublicMethodsFor(Knob.class);
 to print the following list into the console.
 
 You can find further details about class Knob in the javadoc.
 
 Format:
 ClassName : returnType methodName(parameter type)
 
 controlP5.Knob : Knob setConstrained(boolean) 
 controlP5.Knob : Knob setDragDirection(int) 
 controlP5.Knob : Knob setMax(float) 
 controlP5.Knob : Knob setMin(float) 
 controlP5.Knob : Knob setNumberOfTickMarks(int) 
 controlP5.Knob : Knob setRadius(float) 
 controlP5.Knob : Knob setRange(float) 
 controlP5.Knob : Knob setResolution(float) 
 controlP5.Knob : Knob setScrollSensitivity(float) 
 controlP5.Knob : Knob setSensitivity(float) 
 controlP5.Knob : Knob setShowRange(boolean) 
 controlP5.Knob : Knob setStartAngle(float) 
 controlP5.Knob : Knob setTickMarkLength(int) 
 controlP5.Knob : Knob setTickMarkWeight(float) 
 controlP5.Knob : Knob setValue(float) 
 controlP5.Knob : Knob setViewStyle(int) 
 controlP5.Knob : Knob showTickMarks(boolean) 
 controlP5.Knob : Knob shuffle() 
 controlP5.Knob : Knob snapToTickMarks(boolean) 
 controlP5.Knob : Knob update() 
 controlP5.Knob : boolean isConstrained() 
 controlP5.Knob : boolean isShowRange() 
 controlP5.Knob : boolean isShowTickMarks() 
 controlP5.Knob : float getAngle() 
 controlP5.Knob : float getRadius() 
 controlP5.Knob : float getRange() 
 controlP5.Knob : float getResolution() 
 controlP5.Knob : float getStartAngle() 
 controlP5.Knob : float getTickMarkWeight() 
 controlP5.Knob : float getValue() 
 controlP5.Knob : int getDragDirection() 
 controlP5.Knob : int getNumberOfTickMarks() 
 controlP5.Knob : int getTickMarkLength() 
 controlP5.Knob : int getViewStyle() 
 controlP5.Controller : CColor getColor() 
 controlP5.Controller : ControlBehavior getBehavior() 
 controlP5.Controller : ControlWindow getControlWindow() 
 controlP5.Controller : ControlWindow getWindow() 
 controlP5.Controller : ControllerProperty getProperty(String) 
 controlP5.Controller : ControllerProperty getProperty(String, String) 
 controlP5.Controller : Knob addCallback(CallbackListener) 
 controlP5.Controller : Knob addListener(ControlListener) 
 controlP5.Controller : Knob bringToFront() 
 controlP5.Controller : Knob bringToFront(ControllerInterface) 
 controlP5.Controller : Knob hide() 
 controlP5.Controller : Knob linebreak() 
 controlP5.Controller : Knob listen(boolean) 
 controlP5.Controller : Knob lock() 
 controlP5.Controller : Knob plugTo(Object) 
 controlP5.Controller : Knob plugTo(Object, String) 
 controlP5.Controller : Knob plugTo(Object[]) 
 controlP5.Controller : Knob plugTo(Object[], String) 
 controlP5.Controller : Knob registerProperty(String) 
 controlP5.Controller : Knob registerProperty(String, String) 
 controlP5.Controller : Knob registerTooltip(String) 
 controlP5.Controller : Knob removeBehavior() 
 controlP5.Controller : Knob removeCallback() 
 controlP5.Controller : Knob removeCallback(CallbackListener) 
 controlP5.Controller : Knob removeListener(ControlListener) 
 controlP5.Controller : Knob removeProperty(String) 
 controlP5.Controller : Knob removeProperty(String, String) 
 controlP5.Controller : Knob setArrayValue(float[]) 
 controlP5.Controller : Knob setArrayValue(int, float) 
 controlP5.Controller : Knob setBehavior(ControlBehavior) 
 controlP5.Controller : Knob setBroadcast(boolean) 
 controlP5.Controller : Knob setCaptionLabel(String) 
 controlP5.Controller : Knob setColor(CColor) 
 controlP5.Controller : Knob setColorActive(int) 
 controlP5.Controller : Knob setColorBackground(int) 
 controlP5.Controller : Knob setColorCaptionLabel(int) 
 controlP5.Controller : Knob setColorForeground(int) 
 controlP5.Controller : Knob setColorValueLabel(int) 
 controlP5.Controller : Knob setDecimalPrecision(int) 
 controlP5.Controller : Knob setDefaultValue(float) 
 controlP5.Controller : Knob setHeight(int) 
 controlP5.Controller : Knob setId(int) 
 controlP5.Controller : Knob setImages(PImage, PImage, PImage) 
 controlP5.Controller : Knob setImages(PImage, PImage, PImage, PImage) 
 controlP5.Controller : Knob setLabelVisible(boolean) 
 controlP5.Controller : Knob setLock(boolean) 
 controlP5.Controller : Knob setMax(float) 
 controlP5.Controller : Knob setMin(float) 
 controlP5.Controller : Knob setMouseOver(boolean) 
 controlP5.Controller : Knob setMoveable(boolean) 
 controlP5.Controller : Knob setPosition(PVector) 
 controlP5.Controller : Knob setPosition(float, float) 
 controlP5.Controller : Knob setSize(PImage) 
 controlP5.Controller : Knob setSize(int, int) 
 controlP5.Controller : Knob setStringValue(String) 
 controlP5.Controller : Knob setUpdate(boolean) 
 controlP5.Controller : Knob setValueLabel(String) 
 controlP5.Controller : Knob setView(ControllerView) 
 controlP5.Controller : Knob setVisible(boolean) 
 controlP5.Controller : Knob setWidth(int) 
 controlP5.Controller : Knob show() 
 controlP5.Controller : Knob unlock() 
 controlP5.Controller : Knob unplugFrom(Object) 
 controlP5.Controller : Knob unplugFrom(Object[]) 
 controlP5.Controller : Knob unregisterTooltip() 
 controlP5.Controller : Knob update() 
 controlP5.Controller : Knob updateSize() 
 controlP5.Controller : Label getCaptionLabel() 
 controlP5.Controller : Label getValueLabel() 
 controlP5.Controller : List getControllerPlugList() 
 controlP5.Controller : PImage setImage(PImage) 
 controlP5.Controller : PImage setImage(PImage, int) 
 controlP5.Controller : PVector getAbsolutePosition() 
 controlP5.Controller : PVector getPosition() 
 controlP5.Controller : String getAddress() 
 controlP5.Controller : String getInfo() 
 controlP5.Controller : String getName() 
 controlP5.Controller : String getStringValue() 
 controlP5.Controller : String toString() 
 controlP5.Controller : Tab getTab() 
 controlP5.Controller : boolean isActive() 
 controlP5.Controller : boolean isBroadcast() 
 controlP5.Controller : boolean isInside() 
 controlP5.Controller : boolean isLabelVisible() 
 controlP5.Controller : boolean isListening() 
 controlP5.Controller : boolean isLock() 
 controlP5.Controller : boolean isMouseOver() 
 controlP5.Controller : boolean isMousePressed() 
 controlP5.Controller : boolean isMoveable() 
 controlP5.Controller : boolean isUpdate() 
 controlP5.Controller : boolean isVisible() 
 controlP5.Controller : float getArrayValue(int) 
 controlP5.Controller : float getDefaultValue() 
 controlP5.Controller : float getMax() 
 controlP5.Controller : float getMin() 
 controlP5.Controller : float -=6p[078590-() 
 controlP5.Controller : float[] getArrayValue() 
 controlP5.Controller : int getDecimalPrecision() 
 controlP5.Controller : int getHeight() 
 controlP5.Controller : int getId() 
 controlP5.Controller : int getWidth() 
 controlP5.Controller : int listenerSize() 
 controlP5.Controller : void remove() 
 controlP5.Controller : void setView(ControllerView, int) 
 java.lang.Object : String toString() 
 java.lang.Object : boolean equals(Object) 
 
 
 */
