/*
Wireless temperature sensor 
Created for Mark Vladimir Reukov
By Aleksey Shaporev, Clemson University
Started on November, 19 2013
*/
//TODO: eliminate millis!!!! Or make a function millis() for PIC later

//"HEADER"

//types declaration; may be compiler-specific
  typedef long timer_T; 
  typedef int sensor_value_T;  //value read from sensor. As 10-bit ADC - unsigned int16

  typedef signed int si16_T;  //signed int 16, for counters. Somewhere int may be 8-bit by default, or 24-48 bit, no one needs problems with that
  typedef signed char si8_T;  //signed int 8
  typedef signed long si32_T;  //signed int 32
  typedef unsigned char ui8_T;  //unsigned int 8
  typedef int raw_temp_T;    //temperature to be stored in memory. Expected default - signed int8
  
  typedef si8_T circular_buffer_index_T;  //type for circular buffer; affecting the capacity of the buffer - either 255 elements (8-bit), or more
  
   

//! Data acquisition block (DQ)
  //Description:
    //Regularly checks temperature and regularly submits them to data analysis block  
  //Internal functions:
    raw_temp_T  f_DQ_convert_temp(sensor_value_T  sensorValue);  //This function converts signal from the temperature sensor into degrees
  //Incoming signals:
    void f_DQ_timer_updated(timer_T  new_timer);  //called regularly to perform temperature measurements and when necessary - raw data submission for analysis
  //Outgoing signals:
    //f_DA_raw_data_received(raw_data);
  //Variables:
    si16_T v_DQ_timer_counter=0;  //The variable that stores number of f_DQ_timer_updated() executions to only perform temperature test once per 
    const si8_T c_DQ_execution_step=1;  //The constant defining that temperature readings will occur only once per c_DQ_execution_step executions of f_DQ_timer_updated() 
    const int c_DQ_temperature_sensor_pin = A1;  //TODO: move to compiler-specific!!! As well as reading procedure

//! Data analysis block (DA)
  /*
    The purpose of this block is to answer following questions and perform following actions:
    1) Is current ("short term") temperature within pre-defined limits? If no, signal both to UI and BT; leave message in a message buffer as well (for BT)
    2) Is long-term temperature within pre-defined limits? If no, signal to both UI and BT (BT - with a message buffer)
  */
  //Internal functions:
    
  //Incoming signals:
    void f_DA_timer_updated(timer_T  new_timer);
      //Upon receipt: analyze whether short- or long-term steps are completed, if so - process new step
    void f_DA_raw_data_received(raw_temp_T  raw_data);    
      //Add data to DS, check whether new short- or long-term data are generated and respond with corresponding reaction
      
  //Outgoing signals:
    //f_DS_datum_received(data_received);
    //f_UI_start_short_term_temperature_alarm(boolean);
    //f_UI_start_long_term_temperature_alarm(boolean);
    //f_BT_inform_short_term_temperature_alarm(temperature);
    //f_BT_inform_long_term_temperature_alarm(temperature);
    
    //DS_read_values_since(reference_time);  //TODO: implement internal tokens! //TODO: consider better way to do so
    
  //Variables:
    const timer_T c_DA_timer_never = -2000000000;  //TODO: change to -LONG_MAX
    
    timer_T v_DA_last_raw_data_timer=c_DA_timer_never;    //the timestamp value with last data received
    
    /*
      Short-term alarm: it is generated if within v_DA_short_term_alarm_step_interval it was observed that the temperature is lower than
      v_DA_short_term_alarm_low_temp or higher than v_DA_short_term_alarm_high_temp as averaged within v_DA_short_term_alarm_step_interval.
      If it happens - a light alarm is generated, and an BT alarm is sent.
      Difference from long-term alarm - it allows slightly higher volatility of temperatures (combats with 'spikes').        
    */
  /*DEBUG*/   const si16_T  c_DA_short_term_alarm_step_interval_default=10;    //the duration of a single short term data averaging segment - default
    const raw_temp_T  c_DA_short_term_alarm_low_temp_default=10;         //default temperature considered as LOW for short-term alarm
 /*DEBUG*/   const raw_temp_T c_DA_short_term_alarm_high_temp_default=26;         //default temperature considered as HIGH for short-term alarm
    si16_T  v_DA_short_term_alarm_step_interval=c_DA_short_term_alarm_step_interval_default;  //the duration of a single short term data averaging segment
    raw_temp_T  v_DA_short_term_alarm_low_temp=c_DA_short_term_alarm_low_temp_default;  //currently set LOW treshold for short temp alarm
    raw_temp_T  v_DA_short_term_alarm_high_temp=c_DA_short_term_alarm_high_temp_default;  //currently set HIGH treshold for short temp alarm
    timer_T  v_DA_last_short_term_step_timer= c_DA_timer_never;      //the timestamp indicating when last short-term alarm step was completed   
    si16_T  v_DA_short_term_data_counter=0;       //Number of data points averaged since last short-term alarm ster  
    si32_T  v_DA_short_term_data_accumulator=0;  //Sum of all raw data received since last short-term alarm ster 
    
    
    
    /*
      Long-term alarm: it is generated if within v_DA_short_term_alarm_step_interval it was observed that the temperature is lower than
      v_DA_short_term_alarm_low_temp or higher than v_DA_short_term_alarm_high_temp as averaged within v_DA_short_term_alarm_step_interval.
      If it happens - a light alarm is generated, and an BT alarm is sent.
      Difference from long-term alarm - it allows slightly higher volatility of temperatures (combats with 'spikes').        
    */
    const si16_T  c_DA_long_term_alarm_step_interval_default=300;       //the duration of a single long term data averaging segment
    const raw_temp_T  c_DA_long_term_alarm_low_temp_default=14;         //default temperature considered as LOW for long-term alarm
    const raw_temp_T  c_DA_long_term_alarm_high_temp_default=36;         //default temperature considered as HIGH for long-term alarm
    si16_T  v_DA_long_term_alarm_step_interval=c_DA_long_term_alarm_step_interval_default;  //the duration of a single long term data averaging segment
    raw_temp_T  v_DA_long_term_alarm_low_temp=c_DA_long_term_alarm_low_temp_default;  //currently set LOW treshold for long temp alarm
    raw_temp_T  v_DA_long_term_alarm_high_temp=c_DA_long_term_alarm_high_temp_default;  //currently set HIGH treshold for long temp alarm
    timer_T  v_DA_last_long_term_step_timer= c_DA_timer_never;      //the timestamp indicating when last long-term alarm step was completed   
    si16_T  v_DA_long_term_data_counter=0;       //Number of data points averaged since last long-term alarm step
    si32_T  v_DA_long_term_data_accumulator=0;  //Sum of all raw data received since last long-term alarm step
    
    
//! System block (SY)
  //Variables
    timer_T  global_timer=0;    //A variable that stores internal global timer. MAY be accessed globally to avoid unnecessary stack business
    timer_T  global_timer_rest=0; //A variable keeping milliseconds of internal global timer
    timer_T  last_millis=0;      //last ms timer value received during update
    
  //Incoming signals
    void f_SY_loop_updated();  
  //Outgoing signals
    //UI and BT are "real-time" updates since they are time-sensitive. Other ones have 1-sec execution interval
    // void f_DA_timer_updated(timer_T  new_timer); 
    // void f_DQ_timer_updated(timer_T  new_timer);
    // void f_UI_loop_updated();    
    // void f_DS_timer_updated(timer_T  new_timer);
    // void f_BT_loop_updated();
    
    
//! User interface block (UI)
  /*
  This block is responsible for presentation of alarms (short-term and long-term) to user.
  Weak alarm is a "something was wrong in a near past" alarm and is presented as a blinking LED with pulse duration of ... and inter-pulse pause of ...
  Long-term alarm is presented as a blinking LED with pulse duration of ... and inter-pulse pause of ...
  Short-term alarm is presented as a blinking LED with pulse duration of ... and inter-pulse pause of ...
  The alarms have priorities of Short>Long>Weak (so all enabled alarms are processed, but if Short alarm is enabled - neither Long, nor Weak
  alarms will affect the LED. If Short alarm is disabled, then Long will drive the LED, etc. 
  */
  
  //Variables
  const int c_UI_led_pin=4;  //a pin to which LED is connected //TODO: move this, next 2 lines, and reading into compiler-specific section!!!
  const int c_UI_led_off=LOW;  //constants to store values that should be assigned to c_UI_led_pin to glow/shut down LED
  const int c_UI_led_on=HIGH;  //it is important since for some devices you need to output LOW to LED to glow it.
  
  typedef struct Alarm 
  {    //this structure defines all the alarm parameters
    boolean allowed;    //whether alarm should affect LED at all, or not
    boolean enabled;    //whether alarm is launched or not (LED may be on or off, depending on a cycle)
    boolean led_on;     //whether in the current cycle LED is on
    timer_T  toggle_timer;  //time when LED should be toggled (millis!!!)
    si16_T  on_duration;    //duration of an ON cycle  (millis!!!)
    si16_T  off_duration;   //duration of an OFF cycle  (millis!!!)
    timer_T  disable_timer;    //time when to disable the alarm (seconds!!!)
  } Alarm;
  const si8_T  c_UI_number_of_alarms=3; //Short, long, weak
  const si8_T  c_UI_short_alarm=2;  //aliases to access arrays elements
  const si8_T  c_UI_long_alarm=1;
  const si8_T  c_UI_weak_alarm=0;

  const Alarm c_UI_default_alarms[c_UI_number_of_alarms]=    //Default values for different alarms
  {
      {true,false,false,0,500,10000,2000000000},
      {true,false,false,0,1000,2000,2000000000},
      {true,false,false,0,250,250,2000000000}
  };
  Alarm v_UI_alarms[c_UI_number_of_alarms]; //array with all alarms parameters
  const timer_T  c_UI_short_term_alarm_duration_default=c_DA_short_term_alarm_step_interval_default;  //default durations of alarms
  const timer_T  c_UI_long_term_alarm_duration_default=c_DA_long_term_alarm_step_interval_default;
  const timer_T  c_UI_weak_alarm_duration_default=c_DA_long_term_alarm_step_interval_default*10;
  timer_T  v_UI_short_term_alarm_duration=c_UI_short_term_alarm_duration_default;  //actual durations of alarms
  timer_T  v_UI_long_term_alarm_duration=c_UI_long_term_alarm_duration_default;
  timer_T  v_UI_weak_alarm_duration=c_UI_weak_alarm_duration_default;
  timer_T  v_UI_current_millis=millis();    //TODO!!! - fix millis()!!!
  
  
  //Incoming signals
  void f_UI_loop_updated();  //update of alarms every loop 
  void f_UI_start_short_term_temperature_alarm(boolean enable_alarm);
  void f_UI_start_long_term_temperature_alarm(boolean enable_alarm);
  void f_UI_disable_all_alarms();
  
  
//! Data storage block (DS)

  //Variables
  const raw_temp_T c_DS_invalid_data=-100;    //Stands for "invalid data" - e.g. when no data were recorded during a step. 
  const timer_T  c_DS_significant_timer_difference=10;  //treshold for timers to be considered significantly different
  
  typedef struct CircularBuffer  //this is a type for various circular buffers
  {
    circular_buffer_index_T  length_max;  //capacity of a buffer
    circular_buffer_index_T  start;  //when current will become equal to start, next CB data will be added
    circular_buffer_index_T  current;  //the number of record to which data WILL be written upon receipt of a new datum
    raw_temp_T  * data;  //array with data
    circular_buffer_index_T  length;  //number of actually loaded values
    circular_buffer_index_T  counter;  //number of data points actually recorded towards generation of higher level buffer data
    circular_buffer_index_T  counter_max;  //number of data points after which higher buffer data should be generated
      /*concept of counter/counter_max: as soon as new datum is received, counter is increased by 1. 
        When counter reaches counter_max, it is turned to zero, and a higher level buffer data is generated.
        It allows to have circular buffer of higher capacity than number of averaged data points (counter_max)
      */
    timer_T  time_stamp;
  } CircularBuffer;
  
  const si8_T  c_DS_qh=0;  //alias to quarter-hourly buffer
  const si8_T  c_DS_h=1;    //alias to hourly buffer
  const si8_T  c_DS_d=2;    //alias to daily buffer
  const si8_T  c_DS_number_of_buffers=3;   //number of buffers
  
  CircularBuffer v_DS_buffers[c_DS_number_of_buffers];  //array with all circular buffers
  


  /*DEBUG*/
  const si16_T  c_DS_timer_interval=9;  //900 seconds - the interval between timepoints additions to the first CB
  timer_T  v_DS_timer=c_DS_timer_interval;  //a timer after which data will be written to CB. In seconds!
  si16_T  v_DS_counter=0;  //number of data point averaged
  si32_T  v_DS_accumulator=0;  //accumulator of data point received from DA
  
  
  //Incoming signals
  void f_DS_datum_received(raw_temp_T  data_received);    //function from DA with new temperature read
  void f_DS_timer_updated(timer_T  new_timer);        //updater upon timer update

  //TODO: read data from the DS
  
  
  //Outgoing signals
  // void f_BT_new_averaged_data(int data, int buffer_level);    //information to show via bluetooth
  
  //Internal functions
  
  
//! External communications (BT) block
  //Variables
  //idea of the token - token is changed when new data are arrived. The idea is for the phone to know whether any data changed, or not without 
  //all data download - only by knowing if last token it received (and confirmed) is the same as the current token of the device
  #define c_BT_TOKEN_NEVER_CONNECTED 0          //an initial value that can only be set to token (last_confirmed_token) after the start of the device
  #define c_BT_TOKEN_CONNECTION_INIT 1          //an initial value that can only be set to token (v_BT_current_token) after the start of the device
  #define c_BT_TOKEN_START 2                    //limits of token values - tokens are supposed to be between TOKEN_START and TOKEN_MAX
  #define c_BT_TOKEN_MAX 250
  ui8_T  v_BT_current_token = c_BT_TOKEN_CONNECTION_INIT;             //the token value that was generated after last data receipt
  ui8_T  v_BT_last_confirmed_token = c_BT_TOKEN_NEVER_CONNECTED;      //last token value that was confirmed by phone

    //alarm tokens - are tokens informing the phone whether new alarm messages were received, thus helping to sync data erase process
  ui8_T  v_BT_current_token_alarm = c_BT_TOKEN_CONNECTION_INIT;             //the alarm token value that was generated after last data receipt
  ui8_T  v_BT_last_confirmed_token_alarm = c_BT_TOKEN_NEVER_CONNECTED;      //last alarm token value that was confirmed by phone
  const si8_T  c_BT_incoming_buffer_size=20;    //size of an incoming messages buffer
  ui8_T  v_BT_incoming_buffer[c_BT_incoming_buffer_size];  //incoming messages buffer
  si8_T  v_BT_incoming_buffer_start=0;    //position of the start symbol
  si8_T  v_BT_incoming_buffer_last=0;  //position of the last symbol in this circular buffer
  si8_T  v_BT_incoming_buffer_length=0;  //number of symbols in the circular buffer
  

  //Incoming signals
  void f_BT_new_averaged_data(raw_temp_T  data, si8_T  buffer_level);    //new averaged obtained in DS in the circular buffer of level buffer_level
  void f_BT_read_from_serial();  //IMPORTANT! Essential for systems with limited UART buffer (like PIC) to connect an interrupt to it
  void f_BT_loop_updated();  //update of BT (including check for new messages available) upon loop 
  void f_BT_inform_short_term_temperature_alarm(raw_temp_T  temperature);  //short alarm received
  void f_BT_inform_long_term_temperature_alarm(raw_temp_T  temperature);  //long alarm received
  //Outgoing signals
  
  //Internal functions
  ui8_T  f_BT_update_token();  //increase token upon receipt of a new data
  ui8_T  f_BT_update_token_alarm();  //increase alarm token upon receipt of a new data
  ui8_T  f_BT_read_from_buffer();  //reading char from incoming message buffer
  boolean f_BT_incoming_buffer_not_empty();  //returns whether incoming buffer is empty or not
  void f_BT_return_unknown_command();  //sends "Unknown command" to the phone.
  boolean f_BT_serial_available();    //returns true/false based on whether new UART/serial data available for reading or not
      //added for cross-compiler independency
  ui8_T  f_BT_read_char_from_serial();    //reads char from UART/serial. Added for cross-compiler independency. Implementation is compiler-specific
  
//IMPLEMENTATION

//! External communications (BT) block

    //CC!!!
  boolean f_BT_serial_available()    //returns true/false based on whether new UART/serial data available for reading or not
  {
    return Serial.available();  //Arduino
  }
    //CC!!!
  ui8_T  f_BT_read_char_from_serial()    //reads char from UART/serial. Added for cross-compiler independency
  {
    return Serial.read();    //Arduino
  }

    //CC!!!
  void f_BT_return_unknown_command()  //sends "Unknown command" to the phone.
  {
    Serial.println("Unknown command!");
  }
  
  boolean f_BT_incoming_buffer_not_empty()  //returns whether incoming buffer is empty or not
  {
    return (v_BT_incoming_buffer_length!=0);  //by simply comparison of the length with zero.
  }
  
  ui8_T  f_BT_read_from_buffer()  //reading char from incoming message buffer
  {
    if (v_BT_incoming_buffer_length>0)
    {
      ui8_T buf=v_BT_incoming_buffer[v_BT_incoming_buffer_start];  //this is the first char in stack
      v_BT_incoming_buffer_start++;  //moving index of the next char to read to next char in stack
      if (v_BT_incoming_buffer_start>=v_BT_incoming_buffer_length) v_BT_incoming_buffer_start-=v_BT_incoming_buffer_length;  //implementing circular buffer
      v_BT_incoming_buffer_length--;  //reducing the length
      //as a result - first char is removed from the stack (circular buffer)
      return buf; 
    }else
    {
      return 0;  //returning error. Althouth this error must be catched before - buffer emptiness must be checked before calling this function
    }
  }
  
  void f_BT_read_from_serial()
  {  //Adds chars to a buffer, and DOES NOT process them here. Processing will be done later, in a regular loop routine.
     //This function is supposed to be called by an interrupt
    ui8_T  buf=0;    //a buffer variable to store value read from serial
    while (f_BT_serial_available()>0)
    {
      buf=f_BT_read_char_from_serial();  //reading the value
      
        //Storing the value in the buffer. It depends on whether there is something in the buffer or not
      if (v_BT_incoming_buffer_length==0)  //if noting in buffer - no change in last and start!!!
      {
        v_BT_incoming_buffer_length++;
        v_BT_incoming_buffer_last=v_BT_incoming_buffer_start;
        v_BT_incoming_buffer[v_BT_incoming_buffer_last]=buf;  
      }else
      {
        v_BT_incoming_buffer_length++;
        v_BT_incoming_buffer_last++;
        if (v_BT_incoming_buffer_last>=c_BT_incoming_buffer_size) v_BT_incoming_buffer_last-=c_BT_incoming_buffer_size;
        if (v_BT_incoming_buffer_length>c_BT_incoming_buffer_size)
        {  //buffer overflow!!!
          //TODO: decide, what to do in this case. Simplest solution - clear the buffer
          v_BT_incoming_buffer_last=0;  //clearing the buffer
          v_BT_incoming_buffer_start=0;
          v_BT_incoming_buffer_length=1;
        };  
        v_BT_incoming_buffer[v_BT_incoming_buffer_last]=buf; //and storing the received value as the first one here
      }
    }
  }
    
    //CC!!!
  void f_BT_inform_short_term_temperature_alarm(raw_temp_T  temperature)  //short alarm received
  {
    f_BT_update_token_alarm();  //updating alarm token
    //output the alarm to bluetooth
        /*DEBUG*/  Serial.print("New short term alarm with temperature:");
               Serial.print("\t");  
               Serial.print(temperature);
               Serial.print("\t");   
               Serial.print("on:");
               Serial.print("\t");  
               Serial.print(global_timer);
               Serial.print("\t");  
               Serial.print("with token:");
               Serial.print("\t");  
               Serial.println(v_BT_current_token_alarm);
     //TODO: implement circular buffer for short term alarm messages
  }
    
    //CC!!!
  void f_BT_inform_long_term_temperature_alarm(raw_temp_T  temperature)  //long alarm received
  {
    f_BT_update_token_alarm();  //updating alarm token
    //output the alarm to bluetooth
        /*DEBUG*/  Serial.print("New long term alarm with temperature:");
               Serial.print("\t");  
               Serial.print(temperature);
               Serial.print("\t");  
               Serial.print("on:");
               Serial.print("\t");  
               Serial.print(global_timer);
               Serial.print("\t");  
               Serial.print("with token:");
               Serial.print("\t");  
               Serial.println(v_BT_current_token_alarm);
     //TODO: implement circular buffer for long term alarm messages
  }
  
  
      //TODO: fix. Make this function compiler-unspecific (with calls of compiler-specific ones)
  void f_BT_loop_updated()
  {
    ui8_T buf=0;
    while (f_BT_incoming_buffer_not_empty())
    {
      buf=f_BT_read_from_buffer();  //reading char from the buf
      //And in the future the whole BTP protocol will be implemented. Now - only 1-byte no-parameters commands are recognized
      //TODO: fix
      switch (buf){
        case 'Z':   //disable all current alarms   
          f_UI_disable_all_alarms();  //send a message to UI block
          break;
        case 'D':  //read all data

          Serial.print("Current temperature: ");  //outputting current data
          Serial.print("\t");
          if (v_DS_counter>0)  //if any points collected currently - output average temp
          {
            signed long temp_buf=v_DS_accumulator/v_DS_counter;
            Serial.println(temp_buf);
          }else  //otherwise - output "no data collected yet"
          {
            Serial.println("No data collected yet");            
          }
          
          for (int i=0; i<c_DS_number_of_buffers;i++)
          {
            Serial.print("Buffer");
            Serial.print("\t");
            Serial.print(i);
            Serial.print("\t");
            Serial.print("Last updated:");
            Serial.print("\t");
            Serial.print(v_DS_buffers[i].time_stamp);
            Serial.print("\t");
            Serial.print("Number of records:");
            Serial.print("\t");
            Serial.println(v_DS_buffers[i].length);
            int index=v_DS_buffers[i].current-1;
            if (index<0) index+=v_DS_buffers[i].length_max;
            for (int k=0;k<v_DS_buffers[i].length;k++) //output buffer values
            {
              Serial.print(v_DS_buffers[i].data[index]);
              Serial.print("\t");
              index--;
              if (index<0) index+=v_DS_buffers[i].length_max;
            }
            
            Serial.println("\t");
          };
          break;
        default:  //command not recognized
          f_BT_return_unknown_command();  //send "unknown command"
          break;
      };
    };
    
    //TODO: implement reading of external messages
    /*
      TODO:
      External messages to be monitored:
        Read all data
        Confirm token
        Read alarm data
        Confirm alarm token (same as Delete alarm data)
        Disable active alarms
        Set data acquisition timer to ... (to receive data in the beginning of an hour, etc.) (later)
        Change some device parameters (such as timers durations etc.) (later)        
    */
  }

  ui8_T  f_BT_update_token()  //increase token upon receipt of a new data
  {
    v_BT_current_token++;                                                  //as new data received, the token has to be updated
    if (v_BT_current_token==v_BT_last_confirmed_token) v_BT_current_token++;         //this serves as a protection, so that iPhone will not get a token that was recently confirmed
    if (v_BT_current_token>c_BT_TOKEN_MAX)  v_BT_current_token=c_BT_TOKEN_START;                //making sure that token value is withing limits
    if (v_BT_current_token==c_BT_TOKEN_NEVER_CONNECTED) v_BT_current_token=c_BT_TOKEN_START;    //making sure that token value is not TOKEN_NEVER_CONNECTED
    if (v_BT_current_token==c_BT_TOKEN_CONNECTION_INIT) v_BT_current_token=c_BT_TOKEN_START;    //making sure that token value is not TOKEN_CONNECTION_INIT
    return v_BT_current_token;
  }
  
  ui8_T  f_BT_update_token_alarm()  //increase token upon receipt of a new data
  {
    v_BT_current_token_alarm++;                                                  //as new error messages were received, the token has to be updated
    if (v_BT_current_token_alarm==v_BT_last_confirmed_token_alarm) v_BT_current_token_alarm++;         //this serves as a protection, so that iPhone will not get a token that was recently confirmed
    if (v_BT_current_token_alarm>c_BT_TOKEN_MAX)  v_BT_current_token_alarm=c_BT_TOKEN_START;                //making sure that token value is withing limits
    if (v_BT_current_token_alarm==c_BT_TOKEN_NEVER_CONNECTED) v_BT_current_token_alarm=c_BT_TOKEN_START;    //making sure that token value is not TOKEN_NEVER_CONNECTED
    if (v_BT_current_token_alarm==c_BT_TOKEN_CONNECTION_INIT) v_BT_current_token_alarm=c_BT_TOKEN_START;    //making sure that token value is not TOKEN_CONNECTION_INIT
    return v_BT_current_token_alarm;
  }

    //CC!!!
    //TODO: fix, make non-CC
  void f_BT_new_averaged_data(raw_temp_T  data, si8_T  buffer_level)    //new averaged obtained in DS in the circular buffer of level buffer_level
  {
    f_BT_update_token();
    /*DEBUG*/  Serial.print("New data:");
               Serial.print("\t");  
               Serial.print(data);
               Serial.print("\t");  
               Serial.print("in buffer:");
               Serial.print("\t");  
               Serial.print(buffer_level);
               Serial.print("\t");  
               Serial.print("on:");
               Serial.print("\t");  
               Serial.print(global_timer);
               Serial.print("\t");  
               Serial.print("with token:");
               Serial.print("\t");
               Serial.println(v_BT_current_token);
              
  }

//! Data storage block (DS)
  void f_DS_datum_received(raw_temp_T  data_received)    //function from DA with new temperature read
  {
    v_DS_counter++;    //just adding value to the accumulator and increasing counter. Check whether it is time to close session will be performed elsewhere
    v_DS_accumulator+=data_received;  
  }

  void f_DS_timer_updated(timer_T  new_timer)        //updater upon timer update
  {  //this function updates all the buffers if necessary
    if (new_timer>v_DS_timer)  //if it is time to end averaging for quarter-hour data
    {
      if (v_DS_counter>0)  //calculate average temperature, if any data received
      {
        v_DS_accumulator/=v_DS_counter;
      }else  //otherwise write "invalid data"
      {
        v_DS_accumulator=c_DS_invalid_data;
      }
      
      for (si8_T  i=0;i<c_DS_number_of_buffers;i++) //adding data to buffers. If a buffer must update higher level buffer - it will be done here
      {
        f_BT_new_averaged_data(v_DS_accumulator, i);
        v_DS_buffers[i].data[v_DS_buffers[i].current]=v_DS_accumulator;  //storing datum in the data
        v_DS_buffers[i].current++;  //increasing position of the next datum to write
        v_DS_buffers[i].current%=v_DS_buffers[i].length_max;  //implementing circulation of the buffer
        v_DS_buffers[i].length++;  //increasing number of data stored in the array
        if (v_DS_buffers[i].length>v_DS_buffers[i].length_max) v_DS_buffers[i].length=v_DS_buffers[i].length_max;  //if it exceeds buffer capacity - it obviously must be set to buffer capacity
        v_DS_buffers[i].time_stamp=new_timer;  //setting time stamp of the last data stored
        v_DS_buffers[i].counter++;  //increasing number of points counting towards generation of high-level data
        v_DS_buffers[i].counter%=v_DS_buffers[i].counter_max;  //if it is higher than counter_max - convert it to zero and generate higher level buffer data
        if ((v_DS_buffers[i].counter==0)&&(v_DS_buffers[i].length>=v_DS_buffers[i].counter_max))
          //two conditions must be met for generation of higher-level buffer data: counter reached counter_max (thus converted to 0); 
          //and number of data points must be greater or equal to counter_max
        {//so new datum must be added to next buffer
          v_DS_accumulator=0;
          v_DS_counter=0;
          for (circular_buffer_index_T  k=0; k<v_DS_buffers[i].length;k++)  //calculating new datum as an average of all data in the buffer
          {
            if (v_DS_buffers[i].data[k]!=c_DS_invalid_data)  //skipping invalid data points
            {
              v_DS_counter++;
              v_DS_accumulator+=v_DS_buffers[i].data[k];  
            };
          };
          if (v_DS_counter>0)  //calculate average temperature, if any data found
          {
            v_DS_accumulator/=v_DS_counter;
          }else  //otherwise write "invalid data"
          {
            v_DS_accumulator=c_DS_invalid_data;
          }
          
        }else
        {//no modification of a higher-level buffer is required, so quitting
          break;
        };        
      };
      
      v_DS_counter=0;  //clearing counter and accumulator for the next QH step
      v_DS_accumulator=0;
      if (new_timer-v_DS_timer<c_DS_significant_timer_difference)    //this is to check whether there is no significant difference between target timer and global_timer
      {//if no - everything ok, and just extending the timer
        v_DS_timer+=c_DS_timer_interval;
      }else
      {//else - somehow the global_timer changed significantly, so setting the new timer to gt+step
        v_DS_timer=new_timer+c_DS_timer_interval; 
      }
    }
  }
  
//! User interface block (UI)
  void f_UI_start_short_term_temperature_alarm(boolean enable_alarm)  //start/stop short alarm
  {
    v_UI_current_millis=millis();
    v_UI_alarms[c_UI_short_alarm].enabled=enable_alarm;    //enable/disable it
    v_UI_alarms[c_UI_short_alarm].led_on=enable_alarm;     //start LED state: ON/OFF based on received parameter
    v_UI_alarms[c_UI_short_alarm].toggle_timer=v_UI_current_millis+ v_UI_alarms[c_UI_short_alarm].on_duration;  //setting next toggle timer
    v_UI_alarms[c_UI_short_alarm].disable_timer=global_timer+v_UI_short_term_alarm_duration;  //setting alarm disable timer
    
    if (!enable_alarm) return;    //if alarm should be enabled - a weak alarm must be enabled too.  
     //and enableing weak alarm - so that after short alarm is expired, a weak alarm keeps working
    v_UI_alarms[c_UI_weak_alarm].enabled=true;    //enable it
    v_UI_alarms[c_UI_weak_alarm].led_on=true;     //start LED state: ON
    v_UI_alarms[c_UI_weak_alarm].toggle_timer=v_UI_current_millis+ v_UI_alarms[c_UI_weak_alarm].on_duration;  //setting next toggle timer
    v_UI_alarms[c_UI_weak_alarm].disable_timer=global_timer+v_UI_weak_alarm_duration+v_UI_short_term_alarm_duration;  //setting alarm disable timer
  }
  
  void f_UI_start_long_term_temperature_alarm(boolean enable_alarm)  //start/stop long alarm
  {
    v_UI_current_millis=millis();
    v_UI_alarms[c_UI_long_alarm].enabled=enable_alarm;    //enable/disable it
    v_UI_alarms[c_UI_long_alarm].led_on=enable_alarm;     //start LED state: ON/OFF based on received parameter
    v_UI_alarms[c_UI_long_alarm].toggle_timer=v_UI_current_millis+ v_UI_alarms[c_UI_long_alarm].on_duration;  //setting next toggle timer
    v_UI_alarms[c_UI_long_alarm].disable_timer=global_timer+v_UI_long_term_alarm_duration;  //setting alarm disable timer
    
    if (!enable_alarm) return;    //if alarm should be enabled - a weak alarm must be enabled too.
    //and enableing weak alarm - so that after long alarm is expired, a weak alarm keeps working
    v_UI_alarms[c_UI_weak_alarm].enabled=true;    //enable it
    v_UI_alarms[c_UI_weak_alarm].led_on=true;     //start LED state: ON
    v_UI_alarms[c_UI_weak_alarm].toggle_timer=v_UI_current_millis+ v_UI_alarms[c_UI_weak_alarm].on_duration;  //setting next toggle timer
    v_UI_alarms[c_UI_weak_alarm].disable_timer=global_timer+v_UI_weak_alarm_duration+v_UI_long_term_alarm_duration;  //setting alarm disable timer
  }

  void f_UI_disable_all_alarms()  //disable all alarms based on externally received message
  {
    for (ui8_T  i=0;i<c_UI_number_of_alarms; i++)  //disableing all alarms
    {
      v_UI_alarms[i].enabled=false;  
      v_UI_alarms[i].led_on=false;
    }
    digitalWrite(c_UI_led_pin,c_UI_led_off);  //quenching LED.  //TODO: fix for CC!!!!
  }


  void f_UI_loop_updated()  //update of alarms every loop
  {  //update all alarms, and update LED state based on them
    v_UI_current_millis=millis();
    boolean v_UI_buf_led_on=false;
    //alarms and LED update:
    for (ui8_T  i=0; i<c_UI_number_of_alarms; i++)
    {
      //alarms update
      if (v_UI_alarms[i].disable_timer<global_timer)  //if it is time to disable alarm - do it
      {
        v_UI_alarms[i].enabled=false;
        v_UI_alarms[i].led_on=false; 
      };
      //LED update
      if (!v_UI_alarms[i].allowed) continue;  //if the alarm is not allowed or not enabled - it does not affect led, so continue
      if (!v_UI_alarms[i].enabled) continue;
      if ((v_UI_current_millis-v_UI_alarms[i].toggle_timer)>0) //if the it is time to toggle
        //! important! (a-b>0), not a>b. This will work in case of overflown timer (when the millis timer becomes negative)
      {
        v_UI_alarms[i].led_on=!v_UI_alarms[i].led_on;  //toggle the LED
        if (v_UI_alarms[i].led_on==true)  //increasing timer based on whether LED is on or off
        {
          v_UI_alarms[i].toggle_timer=v_UI_current_millis+v_UI_alarms[i].on_duration;  
        }else
        {
          v_UI_alarms[i].toggle_timer=v_UI_current_millis+v_UI_alarms[i].off_duration;
        };  
      }
      v_UI_buf_led_on=v_UI_alarms[i].led_on;  //this will update led state accordingly to each alarm (enabled and allowed)
        //but it is OK, since the alarms priority increases within array from 0-th element to 2-nd 
    }
    if (v_UI_buf_led_on)    //outputting obtained desired LED state to the LED pin.
    {
      digitalWrite(c_UI_led_pin,c_UI_led_on);    //TODO: fix for CC
    }else
    {
      digitalWrite(c_UI_led_pin,c_UI_led_off);    //TODO: fix for CC
    };
  }

//! System block (SY)
  void f_SY_loop_updated()
  {
    //Updating global_timer  //TODO: move into an independent compiler-specific function
    
    timer_T  new_millis=millis();
    timer_T  old_global_timer=global_timer;    //this variable is used to check whether global_timer changed or not
    global_timer+=(new_millis-last_millis)/1000;  //first, adding integer seconds passed since last function call. Remember that millis() gives
                                                  //timestamp in milliseconds
    global_timer_rest+=(new_millis-last_millis)%1000;  //the remaining milliseconds are added to global_timer_rest
    global_timer+=global_timer_rest/1000;          // and if it becomes greater than 1000 ms - all integer seconds are added to global_timer
    global_timer_rest%=1000;                       // and only remaining milliseconds are kept in the global_timer_rest
    last_millis=new_millis;    //saving timer in last_millis
    
    //Launching all dependent outgoing signals
    if (old_global_timer!=global_timer)  //global_timer changed, and we need to call all second-interval functions
    {
      f_DA_timer_updated(global_timer); 
      f_DQ_timer_updated(global_timer);
      f_DS_timer_updated(global_timer);
    }
    if (f_BT_serial_available()>0) f_BT_read_from_serial();  //will be changed to an interrupt in a PIC firmware
    f_UI_loop_updated();   //and calling all "real-time" functions as well
    f_BT_loop_updated();  
  }


//! Data acquisition block (DQ)
  void f_DQ_timer_updated(timer_T  new_timer)
  {
    v_DQ_timer_counter++;            //increasing counter of this function calls
    v_DQ_timer_counter%=c_DQ_execution_step;  //checking if this counter reached treshold for temperature measurement
    if (v_DQ_timer_counter>0) return;  //if treshold not met - do nothing, otherwise continue - perform temperature measurement and submit data to DA
    int datum=f_DQ_convert_temp(analogRead(c_DQ_temperature_sensor_pin));    //reading the temperature
      //TODO: fix compiler-specificity
    f_DA_raw_data_received(datum);        //sending data to DA
  }
  
  //!!!CC 
  //!!! Hardware-specific
  raw_temp_T  f_DQ_convert_temp(sensor_value_T  sensorValue)  //This function converts signal from the temperature sensor into degrees
    //taken from seeedstudio.com as a part of specifications of a sensor used in the current project. 
    //Details can be found at: http://www.seeedstudio.com/wiki/index.php?title=GROVE_-_Starter_Kit_v1.1b#Grove_-_Temperature_Sensor
  {
    const int B=3975; 
    double TEMP;
    float Rsensor;
    Rsensor=(float)(1023-sensorValue)*10000/sensorValue;
    TEMP=1/(log(Rsensor/10000)/B+1/298.15)-273.15;
    return (raw_temp_T) TEMP;
  }


//! Data analysis block (DA)

void f_DA_raw_data_received(raw_temp_T  raw_data)  //The function is called when DQ has received a new datum
  //this function only adds data to accumulators and sends it to DS block, and DOES not analyze short- or long-term alarms
  //Such analysis is performed upon DA timer update.
{
  f_DS_datum_received(raw_data);    //storing the data in the DS block
  v_DA_last_raw_data_timer=global_timer;    //updating internal raw data receipt timestamp
  if (v_DA_last_short_term_step_timer==c_DA_timer_never)  //if no short-term steps were started yet - start one
  {
    v_DA_last_short_term_step_timer=global_timer; 
    v_DA_short_term_data_counter=0;
    v_DA_short_term_data_accumulator=0;
  }
  if (v_DA_last_long_term_step_timer==c_DA_timer_never)  //if no short-term steps were started yet - start one
  {
    v_DA_last_long_term_step_timer=global_timer; 
    v_DA_long_term_data_counter=0;
    v_DA_long_term_data_accumulator=0;
  }
  
  v_DA_short_term_data_counter++;  //adding datum to short-term accumulator and increasing the counter
  if (v_DA_short_term_data_counter<0)  //checking for counter overflow. If so - return to previous state, and do not increase accumulator
  {
    v_DA_short_term_data_counter--;  
  }else
  {
    v_DA_short_term_data_accumulator+= raw_data; 
  }
  
  v_DA_long_term_data_counter++;  //adding datum to long-term accumulator and increasing the counter
  if (v_DA_long_term_data_counter<0)  //checking for counter overflow. If so - return to previous state, and do not increase accumulator
  {
    v_DA_long_term_data_counter--;  
  }else
  {
    v_DA_long_term_data_accumulator+= raw_data; 
  }
}

void f_DA_timer_updated(timer_T  new_timer) //Upon receipt: analyze whether short- or long-term steps are completed, if so - process new step
{
  //Analyze whether short-temp step is complete
  if (new_timer>v_DA_last_short_term_step_timer+v_DA_short_term_alarm_step_interval) //the step is complete
  {
    if (v_DA_short_term_data_counter>0)  //Ensure that there were some data acquired during the step
    {
      v_DA_short_term_data_accumulator/=v_DA_short_term_data_counter; //finding average temperature during the step
      if ((v_DA_short_term_data_accumulator<v_DA_short_term_alarm_low_temp)||(v_DA_short_term_data_accumulator>v_DA_short_term_alarm_high_temp))  
          //if conditions for the alarm are met - start the alarm by informing UI and BT
      {
        f_UI_start_short_term_temperature_alarm(true);
        f_BT_inform_short_term_temperature_alarm(v_DA_short_term_data_accumulator);
      }else
      {  //disable UI alarm
        f_UI_start_short_term_temperature_alarm(false);  
      };
    };
    v_DA_last_short_term_step_timer=new_timer;  //setting a new step with current timer as a beginning, and cleared counter and accumulator
    v_DA_short_term_data_counter=0;
    v_DA_short_term_data_accumulator=0;
  }
  
    //Analyze whether long-temp step is complete
  if (new_timer>v_DA_last_long_term_step_timer+v_DA_long_term_alarm_step_interval) //the step is complete
  {
    if (v_DA_long_term_data_counter>0)  //Ensure that there were some data acquired during the step
    {
      v_DA_long_term_data_accumulator/=v_DA_long_term_data_counter; //finding average temperature during the step
      if ((v_DA_long_term_data_accumulator<v_DA_long_term_alarm_low_temp)||(v_DA_long_term_data_accumulator>v_DA_long_term_alarm_high_temp))  
          //if conditions for the alarm are met - start the alarm by informing UI and BT
      {
        f_UI_start_long_term_temperature_alarm(true);
        f_BT_inform_long_term_temperature_alarm(v_DA_long_term_data_accumulator);
      }else
      {  //disable UI alarm
        f_UI_start_long_term_temperature_alarm(false);  
      };
    };
    v_DA_last_long_term_step_timer=new_timer;  //setting a new step with current timer as a beginning, and cleared counter and accumulator
    v_DA_long_term_data_counter=0;
    v_DA_long_term_data_accumulator=0;
  }
  
}
  
  



void setup(){  //TODO: make non-compiler-specific!!!
  //! DQ
    pinMode(c_DQ_temperature_sensor_pin,INPUT_PULLUP);  //setting the temperature pin into INPUT mode with a pullup - in order to avoid random reading in case of sensor connection error
  //!UI
    for (si8_T i=0; i<c_UI_number_of_alarms; i++) v_UI_alarms[i]=c_UI_default_alarms[i];   
    pinMode(c_UI_led_pin,OUTPUT);
    digitalWrite(c_UI_led_pin,c_UI_led_off);
  
  //!DS
        //defining QH circular buffer
    v_DS_buffers[c_DS_qh].length_max=12;
    v_DS_buffers[c_DS_qh].start=0;    
    v_DS_buffers[c_DS_qh].current=0;
    v_DS_buffers[c_DS_qh].length=0; 
    v_DS_buffers[c_DS_qh].time_stamp=0;
    v_DS_buffers[c_DS_qh].counter=0;
    v_DS_buffers[c_DS_qh].counter_max=4;
    v_DS_buffers[c_DS_qh].data=(raw_temp_T*)(malloc(v_DS_buffers[c_DS_qh].length_max * sizeof(raw_temp_T))); 
    
      //defining H circular buffer
    v_DS_buffers[c_DS_h].length_max=24;
    v_DS_buffers[c_DS_h].start=0;
    v_DS_buffers[c_DS_h].current=0;
    v_DS_buffers[c_DS_h].length=0; 
    v_DS_buffers[c_DS_h].time_stamp=0;
    v_DS_buffers[c_DS_h].counter=0;
    v_DS_buffers[c_DS_h].counter_max=24;
    v_DS_buffers[c_DS_h].data=(raw_temp_T*)malloc(v_DS_buffers[c_DS_h].length_max * sizeof(raw_temp_T)); 
    
      //defining D circular buffer
    v_DS_buffers[c_DS_d].length_max=28;
    v_DS_buffers[c_DS_d].start=0;
    v_DS_buffers[c_DS_d].current=0;
    v_DS_buffers[c_DS_d].length=0; 
    v_DS_buffers[c_DS_d].time_stamp=0;
    v_DS_buffers[c_DS_d].counter=0;
    v_DS_buffers[c_DS_d].counter_max=28;
    v_DS_buffers[c_DS_d].data=(raw_temp_T*)malloc(v_DS_buffers[c_DS_d].length_max * sizeof(raw_temp_T));  
    
    for (si8_T i=0; i<c_DS_number_of_buffers;i++)
      for (circular_buffer_index_T k=0; k<v_DS_buffers[i].length_max;k++)
      {
        v_DS_buffers[i].data[k]=-c_DS_invalid_data;  
      }
    
    
    Serial.begin(9600);//DEBUG
  
  
}

void loop(){
  f_SY_loop_updated(); 
}
