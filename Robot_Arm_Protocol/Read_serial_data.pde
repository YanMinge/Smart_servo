
boolean display_switch = true;

void serialEvent(Serial myPort)

{

//  while (myPort.available () > 0) {
//    int inf_from_servo=0;
//    byte[] inf_back = new byte[8]; // back f0 01 10 70 f7
//    int lf = (byte)0xF7;
//   // for(int i = 0;i<=7;i++)
//    //{
//       //inf_back = myPort.readBytes();
//    //}  
//      myPort.readBytes(inf_back);
//    //myPort.readBytesUntil(lf, inf_back);
//
//    if (inf_back != null) {
//      try {
//        if ((inf_back[0] == (byte)START_SYSEX) && (inf_back[2] == (byte)CTL_ERROR_CODE))
//        {  
//          print(inf_back[1]);
//          switch(inf_back[3])//switch error code!
//          {
//            case (byte)PROCESS_SUC :
//            println("success!");
//            break;
//            case (byte)PROCESS_BUSY :
//            println("Servos are busy!");
//            break;
//            case (byte)PROCESS_ERROR:
//            println("Something Wrong While Assigns!");
//            break;
//            case (byte)WRONG_TYPE_OF_SERVICE:
//            println("Wrong Type of Service!");
//            break;
//          }
//        }
//        if ((inf_back[0] == (byte)START_SYSEX)  && (inf_back[2] == (byte)SERVO_TYPE))
//        {
//
//          switch(inf_back[1])
//          {
//          case 1:
//            switch(inf_back[3]) //process get command
//            {
//              case (byte)GET_POSITION:   
//              S1.pos_from_servo = new smart_Servo().receive_Short(inf_back );
//              break;
//              case (byte)GET_SPEED:      
//              S1.speed_from_servo = new smart_Servo().receive_Short(inf_back );
//              break;
//              case (byte)GET_TEMPRATURE: 
//              S1.temp_from_servo = new smart_Servo().calculate_temp(new smart_Servo().receive_Short(inf_back ));
//              break;
//              case (byte)GET_CURRENT:    
//              S1.current_from_servo = new smart_Servo().receive_Short(inf_back );
//              break;
//              case (byte)GET_VOLTAGE:    
//              S1.voltage_from_servo = new smart_Servo().receive_Short(inf_back )*TO_VOLTAGE;
//              break;
//            }
//            break;
//          case 2:
//            switch(inf_back[3]) //process get command
//            {
//              case (byte)GET_POSITION:   
//              S2.pos_from_servo = new smart_Servo().receive_Short(inf_back );
//              break;
//              case (byte)GET_SPEED:      
//              S2.speed_from_servo = new smart_Servo().receive_Short(inf_back );
//              break;
//              case (byte)GET_TEMPRATURE: 
//              S2.temp_from_servo = new smart_Servo().calculate_temp(new smart_Servo().receive_Short(inf_back ));
//              break;
//              case (byte)GET_CURRENT:    
//              S2.current_from_servo = new smart_Servo().receive_Short(inf_back );
//              break;
//              case (byte)GET_VOLTAGE:    
//              S2.voltage_from_servo = new smart_Servo().receive_Short(inf_back )*TO_VOLTAGE;
//              break;
//            }
//            break;
//          }
//        }
//      }
//      catch (Exception e)
//      {
//        //*********************************************************
//      }
//    }
//  }
}

