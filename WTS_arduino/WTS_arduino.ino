/*
Wireless temperature sensor 
Created for Mark Vladimir Reukov
By Aleksey Shaporev, Clemson University
Started on November, 19 2013
*/

//"HEADER"

//! Data acquisition block (DQ)
  //Description:
    //Regularly checks temperature and regularly submits them to data analysis block  
  //Internal functions:
    void f_DQ_timer_updated(long new_timer);  //called regularly to perform temperature measurements and when necessary - raw data submission for analysis
  //Incoming signals:
    //NONE
  //Outgoing signals:
    //f_DA_data_received(raw_data);
  //Variables:
    signed int v_DA_timer_counter=0;  //The variable that stores number of f_DQ_timer_updated() executions to only perform temperature test once per 
    const signed int c_DA_execution_step=32;  //The constant defining that temperature readings will occur only once per c_DA_execution_step executions of f_DQ_timer_updated() 
    const int c_DA_temperature_sensor_pin = A1;  

//! Data analysis block (DA)
  /*
    The purpose of this block is to answer following questions and perform following actions:
    1) Is current ("short term") temperature within pre-defined limits? If no, signal both to UI and BT; leave message in a message buffer as well (for BT)
    2) Is long-term temperature within pre-defined limits? If no, signal to both UI and BT (BT - with a message buffer)
  */
  //Internal functions:
    
  //Incoming signals:
    void f_DA_timer_updated(long new_timer);
      //Upon receipt: analyze whether session has ended, if so - send signal to data storage and UI
    void f_DA_raw_data_received(int raw_data);    
      //Add data to DS, check whether new short- or long-term data are generated and respond with corresponding reaction
      
  //Outgoing signals:
    //f_DS_datum_received(data_received,v_DA_last_raw_data_timer);
    //f_UI_start_short_term_temperature_alarm(boolean);
    //f_UI_start_long_term_temperature_alarm(boolean);
    //f_BT_inform_short_term_temperature_alarm(boolean);
    //f_BT_inform_long_term_temperature_alarm(boolean);
    
    //DS_read_values_since(reference_time);  //TODO: implement internal tokens! //TODO: consider better way to do so
    
  //Variables:
    const signed long c_DA_raw_data_timer_never_received = +2000000000;  //TODO: change to LONG_MAX
    long v_DA_last_raw_data_timer=c_DA_raw_data_timer_never_received;    //the timestamp value with last data received
    const int c_DA_short_term_alarm_step_interval_default=60;    //the duration of a single short term data averaging segment - default
    int v_DA_short_term_alarm_step_interval=c_DA_short_term_alarm_step_interval_default;  //the duration of a single short term data averaging segment
    const int c_DA_short_term_alarm_step_number=5;    //nubmer of records monitored
    const int c_DA_long_term_alarm_step_interval_default=300;       //the duration of a single long term data averaging segment
    int v_DA_long_term_alarm_step_interval=c_DA_long_term_alarm_step_interval_default;  //the duration of a single long term data averaging segment
    const int c_DA_long_term_alarm_step_number=5;    //nubmer of records monitored
    
    
    
    
//IMPLEMENTATION

//! Data acquisition block (DQ)
  void f_DQ_timer_updated(long new_timer)
  {
    v_DA_timer_counter++;            //increasing counter of this function calls
    v_DA_timer_counter%=c_DA_execution_step;  //checking if this counter reached treshold for temperature measurement
    if (v_DA_timer_counter>0) return;  //if treshold not met - do nothing, otherwise continue - perform temperature measurement and submit data to DA
    int datum=analogRead(c_DA_temperature_sensor_pin);    //reading the temperature
    f_DA_data_received(datum);        //sending data to DA
  }


//! Data analysis block (DA)
  void DA_raw_data_received(int raw_data)
  {
    if (DA_session_started==true)
    {
      DA_current_session_counter+=raw_data;
      DA_total_counter+=raw_data;
      DA_last_raw_data_timer=SY_global_timer;  
    }
    else
    {
      DA_session_started=true;
      DA_current_session_counter=0;
      DA_current_session_counter+=raw_data;
      DA_total_counter+=raw_data;
      DA_last_raw_data_timer=SY_global_timer;
    };
    UI_raw_data_received();    
  };
  
  
  void DA_timer_updated(long new_timer)
  {
    if (DA_total_counter>=DA_water_setpoint)
    {
       UI_low_water(false); 
    }else
    {
       UI_low_water(true);
    };
    
    if (DA_session_started==false) return;
    if (DA_last_raw_data_timer<new_timer-DA_session_max_delay) //we need to close a session
    {
      DA_session_started=false;
      DS_session_ended(DA_current_session_counter,DA_last_raw_data_timer);  
      UI_session_ended();
    };
  };
  
  
  void DA_new_water_setpoint_received(long new_setpoint)
  {
    DA_water_setpoint=new_setpoint;  
  };
  
  



void setup(){
  //! DQ
    pinMode(c_DA_temperature_sensor_pin,INPUT_PULLUP);  //setting the temperature pin into INPUT mode with a pullup - in order to avoid random reading in case of sensor connection error
}

void loop(){
}
