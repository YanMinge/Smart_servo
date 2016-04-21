float time_dif = 1;
int number_print = 0,Speed_execute = 10,Speed_Learn = 2;
//void save_learn_data()
//{
//  if (flag_learn_pressed) {
//    if(millis()%100==0)
//      {
//        try {
//          createWriter.println(int_data[0]+","+int_data[1]+","+int_data[2]+","+dif_count);
//          println("Printing");
//        }
//        catch(Exception e) {
//          println("It Broke :/");
//          e.printStackTrace();
//        }
//      }
//  }
//}
void load_learn_data_then_action()
{  
  if (index < loadStrings.length) 
  {
 if (millis()%Speed_execute == 0)
    {
    String[] pieces = split(loadStrings[index], ',');

      if (pieces.length == 2)
      {
        int ID = int(pieces[0]);
        int pos = int(pieces[1]);     
        send_array(ID, pos);
      }
    
    index = index + 1;
    if (index == loadStrings.length){index = 0;}
  }
  }
}
void save_learn_data(int ID, int Pos)
{
  if (flag_learn_pressed) {
    if (millis()%Speed_Learn==0)
    {
      try {
        createWriter.println(ID+","+Pos);
        println("Printing"+number_print);
        number_print++;
      }
      catch(Exception e) {
        println("It Broke :/");
        e.printStackTrace();
      }
    }
  }
}      

