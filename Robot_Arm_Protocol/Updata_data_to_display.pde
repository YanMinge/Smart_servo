void updata_data_display(int ID, int position, int speed, float voltage, float temp, boolean isupdata)
{
  if (isupdata) {
    switch (ID)
    {
    case 1:
      cp5.getController("A_temprature").setValue(temp);
      cp5.getController("A_Voltage").setValue(voltage);
      cp5.getController("A_Pos").setValue(position);
      cp5.getController("A_Speed").setValue(speed);
      break;

    case 2:
      cp5.getController("B_temprature").setValue(temp);
      cp5.getController("B_Voltage").setValue(voltage);
      cp5.getController("B_Pos").setValue(position);
      cp5.getController("B_Speed").setValue(speed);
      break;

    case 3:
      cp5.getController("C_temprature").setValue(temp);
      cp5.getController("C_Voltage").setValue(voltage);
      cp5.getController("C_Pos").setValue(position);
      cp5.getController("C_Speed").setValue(speed);
      break;

    case 4:
      cp5.getController("D_temprature").setValue(temp);
      cp5.getController("D_Voltage").setValue(voltage);
      cp5.getController("D_Pos").setValue(position);
      cp5.getController("D_Speed").setValue(speed);
      break;

    case 5:
      cp5.getController("E_temprature").setValue(temp);
      cp5.getController("E_Voltage").setValue(voltage);
      cp5.getController("E_Pos").setValue(position);
      cp5.getController("E_Speed").setValue(speed);
      break;
    }
  }
}
void update_pos()
{
  switch(updata_servo_id)
  {
  case 1 : 
    //    servo_one_update();
//    S1.handle_receive();
    
    cp5.getController("A_Pos").setValue(S1.pos_from_servo);
    cp5.getController("A_Speed").setValue(S1.current_from_servo);
    cp5.getController("A_Voltage").setValue(S1.voltage_from_servo);
    cp5.getController("A_temprature").setValue(S1.temp_from_servo);
    break;
  case 2: 
    //    servo_two_update();
    cp5.getController("B_Pos").setValue(S2.pos_from_servo);

//    S2.handle_receive();

    break;
  case 3 : 
    //    servo_three_update();
//    S3.handle_receive();
    cp5.getController("C_Pos").setValue(S3.pos_from_servo);


    break;
  case 4 : 
    //    servo_four_update();
//    S4.handle_receive();
    cp5.getController("D_Pos").setValue(S4.pos_from_servo);


    break;
  case 5: 
    //    servo_five_update();
//    S5.handle_receive();
    cp5.getController("E_Pos").setValue(S5.pos_from_servo);


    break;
  }
  // println(updata_servo_id);

}

