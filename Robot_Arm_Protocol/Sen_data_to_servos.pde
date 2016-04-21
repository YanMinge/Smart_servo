int hold_offset = 20;///////////////////////////////////////////////pay attention!
int release_value = 0;
void get_command_from_user_mouse()
{
  push_message_to_servos();
}
void push_message_to_servos()
{  
  if ((flag_lock1 == true||flag_lock3 == true)&&(mouse_wheel_state == -1||mouse_wheel_state == 1)||mousePressed == true)
  {
    if (cp5.getController("A_Pos").isMouseOver()||cp5.getController("A_Speed").isMouseOver()) {
      send_array(1, A_Pos, A_Speed);
    }
    if (cp5.getController("B_Pos").isMouseOver()||cp5.getController("B_Speed").isMouseOver()) {
      send_array(2, B_Pos, B_Speed);
    }
    if (cp5.getController("C_Pos").isMouseOver()||cp5.getController("C_Speed").isMouseOver()) {
      send_array(3, C_Pos, C_Speed);
    }
    if (cp5.getController("D_Pos").isMouseOver()||cp5.getController("D_Speed").isMouseOver()) {
      send_array(4, D_Pos, D_Speed);
    }
    if (cp5.getController("E_Pos").isMouseOver()||cp5.getController("E_Speed").isMouseOver()) {
      send_array(5, E_Pos, E_Speed);
    }
    mouse_wheel_state = 0;
  }
}
void mouseReleased() {
  if (release_value == 0) {
    release_value = 255;
  } else {
    release_value = 0;
  }
}

void send_array(int V_ID, int V_position, int V_speed)
{ 
  //myPort.write("C"+","+"D"+V_ID+","+"A"+V_position+","+"S"+V_speed+"#");
  println("Sending:"+"C"+","+"D"+V_ID+","+"A"+V_position+","+"S"+V_speed+"#");
  
}
void send_array(int V_ID, int V_position)
{ 
     //if(ID == 5){V_position = V_position + hold_offset;}
   // myPort.write("C"+","+"D"+V_ID+","+"A"+V_position+","+"S"+200+"#");
    println("Sending:"+"C"+","+"D"+V_ID+","+"A"+V_position+","+"S"+200+"#");
  
}
void get_current_pos_and_speed(int myID, int mode)
{
  switch (mode)
  {
  case 1:
    println("Switch to mode auto");
    //K,D1,M0  M0 == ask M1 == sel_detect  M2  == auto 
    //myPort.write("K"+","+"D"+myID+","+"M"+2+"#");
    break;// add command here
    
  case 2:
 
   // myPort.write("K"+","+"D"+myID+","+"M"+0+"#");
    println("Switch to mode ask");
    break;//
    
  case 3:
   // myPort.write("K"+","+"D"+myID+","+"M"+1+"#");
    println("Switch to mode selt_detect");
    break;//
  }
  //delay(120);
}

void set_motor_released(int IsReleased, int ID)
{
  //add motor release code here.
  switch(IsReleased)
  {
  case 1:
   // println("true  "+ID);
    //G,D1,R1#R0 stop  R1 release
   // myPort.write("G"+","+"D"+ID+","+"R"+IsReleased+"#");
    break;
  case 0:
   // println("false  "+ID);
    //G,D1,R1#R0 stop  R1 release
   // myPort.write("G"+","+"D"+ID+","+"R"+IsReleased+"#");
    break;
  }
}

void release_all_servos()
{
  for (int i = 1;i<=5;i++)
  {
    set_motor_released(1, i);
    //delay(100);
  }
}
//public  int time_to_get_inf = 0,test = 0;
