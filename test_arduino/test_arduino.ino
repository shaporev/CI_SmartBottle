/*
A first draft of a program layout for smart water bottle application
*/

//"HEADER"

//! Data acquisition block (DQ)
  //Internal functions:
    void DQ_data_acquired(int value);
  
  //Incoming signals:
  
  //Outgoing signals:
    //DA_raw_data_received(raw_data);
    
  //Variables:
      

//! Data analysis block (DA)
  //Internal functions:
    boolean DA_analyze_end_of_session();  //Checks whether session should be ended or not
      //simply check whether it passed DA_session_max_delay seconds from last DA_last_raw_data_timer
    
  //Incoming signals:
    void DA_timer_updated(long new_timer);
      //Upon receipt: analyze whether session has ended, if so - send signal to data storage and UI
    void DA_raw_data_received(int raw_data);    
      //Upon receipt: save data, update DA_last_timer_received and DO NOT analyze the data 
    void DA_new_water_setpoint_received(long new_setpoint); 
      //new setpoint
      
  //Outgoing signals:
    //DS_session_ended(DA_current_session_counter,DA_last_raw_data_timer);
    //UI_session_ended();
    //DS_read_values_since(reference_time);  //TODO: implement internal tokens!
    //UI_raw_data_received();
    //UI_low_water(true/false);
    
  //Variables:
    const signed long DA_raw_data_timer_never_received = +2000000000;  //TODO: change to LONG_MAX
    const signed long DA_session_max_delay=10;  //the number of seconds between sips when session is not closed
    int DA_current_session_counter = 0;
    long DA_total_counter=0;
    long DA_last_raw_data_timer=-DA_raw_data_timer_never_received;
    boolean DA_session_started = false;
    long DA_water_setpoint=0;
    
    
    
//IMPLEMENTATION
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
}

void loop(){
}
