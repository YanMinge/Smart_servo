int release_level = 445;
int release_horizontal = 140;
int toggle_offset = -110;
int sizex = 80, sizey = 20;
int temprature = 80;
//PosRange = map(PosRange,300,3700.0,4096);
void gui()
{
  int menulevelone = 50 ;
  int menuleveltwo = 120 ;
  int menulevelthree = 180 ;

  cp5 = new ControlP5(this);

  //PFont p = createFont("Consolas", 14); 
  //cp5.setControlFont(p, 14);
  
   for (int i=1;i<=5;i++) {
    cp5.addBang("Ser"+i)
       .setPosition(910+i*50, 390)
       .setSize(30, 30)
       .setId(i)
       ;
  }
    cp5.addBang("Ser6")
     .setPosition(1035, 445)
     .setSize(80, 30)
     .setTriggerEvent(Bang.RELEASE)
     .setLabel("ALL_SERVOS")
     ;

 
  cp5.getTab("default")
    .activateEvent(true)
      .setLabel("my default tab")
        .setId(1)
          ;
  c = new ControlTimer();
  t = new Textlabel(cp5, "--", 100, 100);
  c.setSpeedOfTime(1);

  cp5.addToggle("A_Release")
    .setPosition(release_horizontal+toggle_offset, release_level)
      .setSize(sizex, sizey)
        //.setValue(1)
        .registerTooltip("Toggle To Release")
          .setMode(ControlP5.SWITCH)
            .getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE)


              ;
  cp5.addToggle("B_Release")
    .setPosition(release_horizontal*2+toggle_offset, release_level)
      .setSize(sizex, sizey)
        //.setValue(1)
        .registerTooltip("Toggle To Release")
          .setMode(ControlP5.SWITCH)
            .getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE)

              ;
  cp5.addToggle("C_Release")
    .setPosition(release_horizontal*3+toggle_offset, release_level)
      .setSize(sizex, sizey)
        //.setValue(1)
        .registerTooltip("Toggle To Release")
          .setMode(ControlP5.SWITCH)
            .getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE)


              ;
  cp5.addToggle("D_Release")
    .setPosition(release_horizontal*4+toggle_offset, release_level)
      .setSize(sizex, sizey)
        //.setValue(1)
        .registerTooltip("Toggle To Release")
          .setMode(ControlP5.SWITCH)
            .getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE)

              ;
  cp5.addToggle("E_Release")
    .setPosition(release_horizontal*5+toggle_offset, release_level)
      .setSize(sizex, sizey)
        //.setValue(1)
        .registerTooltip("Toggle To Release")
          .setMode(ControlP5.SWITCH)
            .getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE)

              ;  
  myTextarea = cp5.addTextarea("txt")
    .setPosition(700, 340)
      .setSize(250, 160)
        .setFont(createFont("", 14))
          .setLineHeight(14)
            .setColor(color(200))
              .setColorBackground(color(0, 100))
                .setColorForeground(color(255, 100))
                  ;
//console = cp5.addConsole(myTextarea);//

  cp5.addButton("Start_Learn")
    .setValue(0)
      .setPosition(710, menuleveltwo)
        .setColorForeground(color(100))
          .setColorBackground(color(0, 160, 100))
            .setColorActive(color(230, 100, 0))
              .setSize(100, 30)
                ;
  cp5.addButton("Learn_Finish")
    .setValue(100)
      .setPosition(840, menuleveltwo)
        .setColorForeground(color(100))
          .setColorBackground(color(0, 160, 100))
            .setColorActive(color(230, 100, 0))
              .setSize(100, 30)
                ;

  PImage[] imgs_Run = {
    loadImage("button_a.png"), loadImage("button_b.png"), loadImage("button_c.png")
    };
  PImage[] imgs_Stop = {
    loadImage("button_stop_a.png"), loadImage("button_stop_b.png"), loadImage("button_stop_c.png")
    };
    
  cp5.addToggle("Smooth")
    .setPosition(710, menulevelthree+10)
      .setSize(70, 30)
        //.setValue(1)
        .registerTooltip("Toggle Switch To Edit Speed")
          .setMode(ControlP5.SWITCH)
            .getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE)


              ;
    cp5.addButton("run")
      .setValue(128)
        .registerTooltip("Press to Run")
          .setPosition(710+100, menulevelthree)
            .setImages(imgs_Run)
              .updateSize()
                ;

  cp5.addButton("stop")
    .setValue(128)
      .setPosition(840+50, menulevelthree)
        .registerTooltip("Press to Stop")
          .setImages(imgs_Stop)
            .updateSize()
              ;

  cp5.addToggle("Edit_Speed")
    .setPosition(840, menulevelone)
      .setSize(70, 30)
        //.setValue(1)
        .registerTooltip("Toggle Switch To Edit Speed")
          .setMode(ControlP5.SWITCH)
            .getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE)


              ;
  cp5.addToggle("Edit_Pos")
    .setPosition(710, menulevelone)
      .setSize(70, 30)
        //.setValue(1)
        .registerTooltip("Toggle Switch To Edit Position")
          .setMode(ControlP5.SWITCH)
            .getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE)

              ;
  //fill(color(200,30,0));
  //***********************************A_temprature***********************//
  cp5.addSlider("A_temprature")
    .setPosition(offsetX, Temp_distance)
      .setSize(100, 20).setRange(-10, temprature)
        .setValue(0)
          .setSliderMode(1).setScrollSensitivity(0.001)
            .setLock(true);
  ////%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Speed control^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//          
    cp5.addSlider("Speed_Learn")
    .setPosition(710, 265)
      .setSize(100, 20).setRange(1, 10)
        .setValue(2)
         .setNumberOfTickMarks(10)

          .setSliderMode(1).setScrollSensitivity(1);
   cp5.addSlider("Speed_execute")
    .setPosition(710, 305)
      .setSize(100, 20).setRange(1, 20)
        .setValue(2)
         .setNumberOfTickMarks(10)
          .setSliderMode(1).setScrollSensitivity(1);



  cp5.getController("A_temprature")
    .getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE)
      .setPaddingX(0).setPaddingY(0);

  cp5.getController("A_temprature").getCaptionLabel()
    .align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE)
      .setPaddingX(0).setPaddingY(-40);
  //*************************A_Voltage******************// 
  cp5.addSlider("A_Voltage")
    .setPosition(offsetX, 390)
      .setSize(100, 20).setRange(3, 9)
        .setSliderMode(1).setScrollSensitivity(0.001) 
          .setLock(true);
  ;
  cp5.getController("A_Voltage")
    .getValueLabel().align(ControlP5
      .CENTER, ControlP5.BOTTOM_OUTSIDE)
      .setPaddingX(0).setPaddingY(0);
  cp5.getController("A_Voltage")
    .getCaptionLabel().align(ControlP5
      .CENTER, ControlP5.BOTTOM_OUTSIDE)
      .setPaddingX(0)
        .setPaddingY(-40);
  //***********************************A_temprature***********************//
  cp5.addSlider("B_temprature")
    .setPosition(offsetX*3+Radius*2, Temp_distance)
      .setSize(100, 20).setRange(-10, temprature)
          .setValue(0)
            .setValue(0)
              .setSliderMode(1)
                .setScrollSensitivity(0.001) 
                  .setLock(true);
  ;
  cp5.getController("B_temprature")
    .getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE)
      .setPaddingX(0).setPaddingY(0);
  cp5.getController("B_temprature")
    .getCaptionLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE)
      .setPaddingX(0).setPaddingY(-40);
  //*************************A_Voltage******************// 
  cp5.addSlider("B_Voltage")
    .setPosition(offsetX*3+Radius*2, 395)
      .setSize(100, 20).setRange(3, 9)
        .setValue(0)
          .setSliderMode(1)
            .setScrollSensitivity(0.001) 
              .setLock(true);
  ;
  cp5.getController("B_Voltage").getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0).setPaddingY(0);
  cp5.getController("B_Voltage").getCaptionLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0).setPaddingY(-40);
  //***********************************A_temprature***********************//
  cp5.addSlider("C_temprature").setPosition(offsetX*5+Radius*4, Temp_distance).setSize(100, 20).setRange(-10, temprature).setValue(0).setSliderMode(1).setScrollSensitivity(0.001).setLock(true);
  ;
  cp5.getController("C_temprature").getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0).setPaddingY(0);
  cp5.getController("C_temprature").getCaptionLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0).setPaddingY(-40);
  //*************************A_Voltage******************// 
  cp5.addSlider("C_Voltage").setPosition(offsetX*5+Radius*4, 395).setSize(100, 20).setRange(3, 9).setValue(0).setSliderMode(1).setScrollSensitivity(0.001) .setLock(true);
  ;
  cp5.getController("C_Voltage").getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0).setPaddingY(0);
  cp5.getController("C_Voltage").getCaptionLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0).setPaddingY(-40);
  //***********************************A_temprature***********************//
  cp5.addSlider("D_temprature").setPosition(offsetX*7+Radius*6, Temp_distance).setSize(100, 20).setRange(-10, temprature).setValue(0).setSliderMode(1).setScrollSensitivity(0.001) .setLock(true);
  ;
  cp5.getController("D_temprature").getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0).setPaddingY(0);
  cp5.getController("D_temprature").getCaptionLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0).setPaddingY(-40);
  //*************************A_Voltage******************// 
  cp5.addSlider("D_Voltage").setPosition(offsetX*7+Radius*6, 395).setSize(100, 20).setRange(3, 9).setValue(0).setSliderMode(1).setScrollSensitivity(0.001) .setLock(true);
  ;
  cp5.getController("D_Voltage").getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0).setPaddingY(0);
  cp5.getController("D_Voltage").getCaptionLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0).setPaddingY(-40);
  //***********************************A_temprature***********************//
  cp5.addSlider("E_temprature").setPosition(offsetX*9+Radius*8, Temp_distance).setSize(100, 20).setRange(-10, temprature).setValue(0).setSliderMode(1).setScrollSensitivity(0.001) .setLock(true);
  ;
  cp5.getController("E_temprature").getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0).setPaddingY(0);
  cp5.getController("E_temprature").getCaptionLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0).setPaddingY(-40);
  //*************************A_Voltage******************// 
  cp5.addSlider("E_Voltage").setPosition(offsetX*9+Radius*8, 395).setSize(100, 20).setRange(3, 9).setValue(0).setSliderMode(1).setScrollSensitivity(0.001) .setLock(true);
  ;
  cp5.getController("E_Voltage").getValueLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0).setPaddingY(0);
  cp5.getController("E_Voltage").getCaptionLabel().align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0).setPaddingY(-40);
  ///**********************************Section   AAAAA  **********************************************//
  myKnobA_Speed = cp5.addKnob("A_Pos")
    .setCaptionLabel("Servo A Pos") 
      .setRange(0, PosRange)
        //.setDisplayStyle( 1 )
        .setValue(pos_default_value)
          .setPosition(currentX, currentY)
            .setRadius(Radius)
              .setColorActive(color(0, 160, 100))
                .setLock(unlockPos);
  ;

  myKnobA_Pos = cp5.addKnob("A_Speed")
    .setCaptionLabel("Servo A Speed") 
      .setRange(speed_start, speed_range)
        .setValue(defalt_Speed)
          .setPosition(currentX, currentY+Radius*2+vertical)
            .setRadius(Radius)
              .setNumberOfTickMarks(10)
                .setTickMarkLength(4)
                  //.snapToTickMarks(true)
                  .setColorForeground(color(255))
                    .setColorBackground(color(0, 160, 100))
                      .setColorActive(color(255, 255, 0))
                        .setLock(unlockSpeed);
  // .setDragDirection(Knob.HORIZONTAL)
  ;
  ///**********************************Section   BBBBB  **********************************************//
  myKnobB_Speed = cp5.addKnob("B_Pos")
    .setCaptionLabel("Servo B Pos") 
      .setRange(0, 4096)
        .setValue(pos_default_value)
          .setColorActive(color(0, 160, 100))
            .setPosition(currentX+Radius*2+offsetX*2, currentY)
              .setRadius(Radius)
                .setLock(unlockPos);


  ;

  myKnobB_Pos = cp5.addKnob("B_Speed")
    .setCaptionLabel("Servo B Speed") 
      .setRange(speed_start, speed_range)
        .setValue(defalt_Speed)
          .setPosition(currentX+Radius*2+offsetX*2, currentY+Radius*2+vertical)
            .setRadius(Radius)
              .setNumberOfTickMarks(10)
                .setTickMarkLength(4)
                  .snapToTickMarks(false)
                    .setColorForeground(color(255))
                      .setColorBackground(color(0, 160, 100))
                        .setColorActive(color(255, 255, 0))
                          .setLock(unlockSpeed);
  // .setDragDirection(Knob.HORIZONTAL)
  ;
  ///**********************************Section   CCCCC  **********************************************//
  myKnobC_Speed = cp5.addKnob("C_Pos")
    .setCaptionLabel("Servo C Pos") 
      .setRange(300, 3148)
        .setValue(pos_default_value)
          .setColorActive(color(0, 160, 100))
            .setPosition(currentX+Radius*4+offsetX*4, currentY)
              .setRadius(Radius)
                .setLock(unlockPos);

  ;

  myKnobC_Pos = cp5.addKnob("C_Speed")
    .setCaptionLabel("Servo C Speed") 
      .setRange(speed_start, speed_range)
        .setValue(defalt_Speed)
          .setPosition(currentX+Radius*4+offsetX*4, currentY+Radius*2+vertical)
            .setRadius(Radius)
              .setNumberOfTickMarks(10)
                .setTickMarkLength(4)
                  .snapToTickMarks(false)
                    .setColorForeground(color(255))
                      .setColorBackground(color(0, 160, 100))
                        .setColorActive(color(255, 255, 0))
                          .setLock(unlockSpeed);
  // .setDragDirection(Knob.HORIZONTAL)
  ;
  ///**********************************Section   DDDDD  **********************************************//
  myKnobD_Speed = cp5.addKnob("D_Pos")
    .setCaptionLabel("Servo D Pos") 
      .setRange(1165, 3307)
        .setValue(pos_default_value)
          .setColorActive(color(0, 160, 100))
            .setPosition(currentX+Radius*6+offsetX*6, currentY)
              .setRadius(Radius)
                .setLock(unlockPos);

  ;

  myKnobD_Pos = cp5.addKnob("D_Speed")
    .setCaptionLabel("Servo D Speed") 
      .setRange(speed_start, speed_range)
        .setValue(defalt_Speed)
          //.setStartAngle(-PI/2) 
          .setPosition(currentX+Radius*6+offsetX*6, currentY+Radius*2+vertical)
            .setRadius(Radius)
              .setNumberOfTickMarks(10)
                .setTickMarkLength(4)
                  .snapToTickMarks(false)
                    .setColorForeground(color(255))
                      .setColorBackground(color(0, 160, 100))
                        .setColorActive(color(255, 255, 0))
                          .setLock(unlockSpeed);
  // .setDragDirection(Knob.HORIZONTAL)
  ;  
  ///**********************************Section   EEEEE  **********************************************//
  myKnobE_Speed = cp5.addKnob("E_Pos")
    .setCaptionLabel("Servo E Pos") 
      .setRange(300, 3700)
        .setValue(pos_default_value)
          .setColorActive(color(0, 160, 100))
            .setPosition(currentX+Radius*8+offsetX*8, currentY)
              .setRadius(Radius)
                .setLock(unlockPos);

  ;

  myKnobE_Pos = cp5.addKnob("E_Speed")
    .setCaptionLabel("Servo E Speed") 
      .setRange(speed_start, speed_range)
        .setValue(defalt_Speed)
          .setPosition(currentX+Radius*8+offsetX*8, currentY+Radius*2+vertical)
            .setRadius(Radius)
              .setNumberOfTickMarks(10)
                .setTickMarkLength(4)
                  .snapToTickMarks(false)
                    .setColorForeground(color(255))
                      .setColorBackground(color(0, 160, 100))
                        .setColorActive(color(255, 255, 0))
                          .setLock(unlockSpeed);
  // .setDragDirection(Knob.HORIZONTAL)
  ;
}
