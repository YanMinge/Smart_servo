float waring_temprature = 45;
color the_color_overtemprature(int ID)
{
  float temprature_in = 0;
  color warning = color(255,0,0);
  switch (ID)
  {
    case 1 :   temprature_in = cp5.getController("A_temprature").getValue();
    case 2 :   temprature_in = cp5.getController("B_temprature").getValue();
    case 3 :   temprature_in = cp5.getController("C_temprature").getValue();
    case 4 :   temprature_in = cp5.getController("D_temprature").getValue();
    case 5 :   temprature_in = cp5.getController("E_temprature").getValue();
  }
  if (temprature_in > waring_temprature)
  {
    warning = color(temprature_in,temprature_in/10,temprature_in/5);
  }
  return warning;
}
void color_overtemprature()
{
   cp5.getController("A_temprature").getValue();
   cp5.getController("A_temprature").getValue();
   cp5.getController("A_temprature").getValue();
   cp5.getController("A_temprature").getValue();
   cp5.getController("A_temprature").getValue();
}


