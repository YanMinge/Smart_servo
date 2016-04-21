void initialize()
{
  //set servo speed 
  for(int servo_number = 1;servo_number<=4;servo_number++)
  {
    myPort.write("C"+","+"D"+servo_number+","+"A"+2000+","+"S"+125+"#");
    //delay(200);
    myPort.write("C"+","+"D"+5+","+"A"+2413+","+"S"+255+"#");

  }
}
