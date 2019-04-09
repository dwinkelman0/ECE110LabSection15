/******************************************************************************
 * Messaging protocol
 * 
 * 8-bit message is sent
 * _ _ _, _ _ _ _ _
 * MSB          LSB
 * 
 * 3 MSB are the robot_id, 5 LSB are the message
 * 
 * If the robot_id is 7, this indicates a "score received" confirmation
 * message. The values are packed in base-5 number such that the ones-place is
 * robot who sent the confirmation message ("confirmer") and the fives-place is the robot
 * whose score was confirmed to have been received ("confirmee")
 * 
 * If the robot_id is 6, this indicates a "done" message. The data is the ID
 * of the robot that sent the message
 * 
 * A robot (e.g. Chaser 1) will broadcast its own data until Chaser 1 receives
 * confirmation messages that every other robot has received its data
 * 
 * At the point that a robot (e.g. Chaser 1) has received data from any robot (e.g. Beater 1), Chaser 1 will
 * broadcast a confirmation message for Beater 1 until it receives a "done" message from Beater 1
 * 
 * At the point that a robot (e.g. Chaser 1) has received data from every other robot,
 * Chaser 1 will calculate and display the final score and broadcast a "done" message until it receives "done" messages from
 * every other robot
 */

// Comment out to disable serial debug stream
#define DEBUG

// Number of milliseconds to listen between each message broadcast
#define LISTEN_DELAY 500

// Codes for confirm and done messages
#define ROBOT_ID_CONFIRM 7
#define ROBOT_ID_DONE 6

/**
 * Robot ID: the robot sending the data
 * Data: the robot-specific data to send
 */
char make_data_message(int robot_id, int data) {
  return (robot_id & 7) << 5 | (data & 31);
}

/**
 * Robot ID: the robot sending the data
 * Data: the robot-specific data to send
 */
void interpret_data_message(char message, int * robot_id, int * data) {
  if (robot_id) *robot_id = (message >> 5) & 7;
  if (data) *data = message & 31;
}

/**
 * Confirmer: the robot confirming that it received data
 * Confirmee: the robot whose data was received
 */
char make_confirm_data(int confirmer, int confirmee) {
  return confirmee * 5 + confirmer;
}

/**
 * Confirmer: the robot confirming that it received data
 * Confirmee: the robot whose data was received
 */
void interpret_confirm_data(int data, int * confirmer, int * confirmee) {
  if (confirmer) *confirmer = data % 5;
  if (confirmee) *confirmee = data / 5;
}

/**
 * Robot ID: the robot sending the "done" signal
 */
char make_done_message(int robot_id) {
  return make_data_message(ROBOT_ID_DONE, robot_id);
}

/**
 * Robot ID: the robot sending the "done" signal
 */
void interpret_done_message(char message, int * robot_id) {
  interpret_data_message(message, NULL, robot_id);
}

boolean check_all_data(int * data) {
  return data[0] != -1 && data[1] != -1 && data[2] != -1 && data[3] != -1 && data[4] != -1;
}

boolean check_all_confirmed(boolean * data) {
  return data[0] && data[1] && data[2] && data[3] && data[4];
}

boolean check_all_done(boolean * data) {
  return data[0] && data[1] && data[2] && data[3] && data[4];
}

void broadcast(int my_id, char message) {
  Serial2.print(message);

  #ifdef DEBUG
  static char buf[32];
  int robot_id, data;
  interpret_data_message(message, &robot_id, &data);
  if (robot_id == ROBOT_ID_CONFIRM) {
    int confirmer, confirmee;
    interpret_confirm_data(data, &confirmer, &confirmee);
    sprintf(buf, "[%d] Broadcast: Confirm %d", my_id, confirmee);
  }
  else if (robot_id == ROBOT_ID_DONE) {
    sprintf(buf, "[%d] Broadcast: Done", my_id);
  }
  else {
    sprintf(buf, "[%d] Broadcast: Data %d", my_id, data);
  }
  Serial.println(buf);
  #endif
}

char receive_until(int my_id, long t_end) {
  char message = -1;
  while (millis() < t_end && message == -1) {
    if (Serial2.available()) {
      message = Serial2.read();
    }
  }

  #ifdef DEBUG
  if (message != -1) {
    static char buf[32];
    int robot_id, data;
    interpret_data_message(message, &robot_id, &data);
    if (robot_id == ROBOT_ID_CONFIRM) {
      int confirmer, confirmee;
      interpret_confirm_data(data, &confirmer, &confirmee);
      if (confirmee == my_id) {
        sprintf(buf, "[%d] Received [%d]: Confirm", my_id, confirmer);
        Serial.println(buf);
      }
    }
    else if (robot_id == ROBOT_ID_DONE) {
      sprintf(buf, "[%d] Received [%d]: Done", my_id, data);
      Serial.println(buf);
    }
    else {
      sprintf(buf, "[%d] Received [%d]: Data %d", my_id, robot_id, data);
      Serial.println(buf);
    }
  }
  #endif
  
  return message;
}

int score(int my_id, int my_data) {

  #ifdef DEBUG
  static char buf[32];
  #endif

  // Initialize data
  char my_message = make_data_message(my_id, my_data);

  // Data sent by other robots
  int robots_data[5] = {-1, -1, -1, -1, -1};
  robots_data[my_id] = my_data;
  boolean all_data = false;

  // Other robots confirming that they received this robot's data
  boolean robots_confirmed[5] = {false, false, false, false, false};
  robots_confirmed[my_id] = true;
  boolean all_confirmed = false;

  // Other robots declaring themselves done
  boolean robots_done[5] = {false, false, false, false, false};
  robots_done[my_id] = true;
  boolean all_done = false;  

  // Message queue
  char messages[32];
  int n_messages = 0;

  // Broadcast signals
  while (!all_done) {

    memset(messages, sizeof(messages), 0);
    n_messages = 0;

    // Check that all other robots have confirmed they have received the data
    // If not, add a data broadcast to message queue
    if (!all_confirmed) {
      messages[n_messages++] = my_message;
    }

    // Check for data that has been received from other robots
    // For data received, add a confirm broadcast to message queue
    if (!all_data) {
      for (int i = 0; i < 5; i++) {
        if (robots_data[i] != -1 && i != my_id) {
          messages[n_messages++] = make_data_message(ROBOT_ID_CONFIRM, make_confirm_data(my_id, i));
        }
      }
    }

    // If all data received, broadcast done
    if (all_data) {
      messages[n_messages++] = make_done_message(my_id);
    }

    // Broadcast all messages spaced along a time interval
    // Listen when not broadcasting
    for (int i = 0; i < n_messages; i++) {
      broadcast(my_id, messages[i]);

      long t_end = millis() + LISTEN_DELAY;
      char message;
      while ((message = receive_until(my_id, t_end)) != -1) {
        
        // Signal received; change data
        int robot_id, data;
        interpret_data_message(message, &robot_id, &data);
        
        if (robot_id == ROBOT_ID_CONFIRM) {
          int confirmer, confirmee;
          interpret_confirm_data(data, &confirmer, &confirmee);
          if (confirmee == my_id) {
            // A robot confirmed this robot's data was received
            // If this is a new confirmation, check again if all confirmations received
            if (robots_confirmed[confirmer] == false) {
              robots_confirmed[confirmer] = true;
              all_confirmed = check_all_confirmed(robots_confirmed);
              #ifdef DEBUG
              sprintf(buf, "[%d] Confirmation from [%d]", my_id, confirmer);
              Serial.println(buf);
              if (all_confirmed) {
                sprintf(buf, "[%d] --- All Confirmed ---", my_id);
                Serial.println(buf);
              }
              #endif
            }
          }
        }
        
        else if (robot_id == ROBOT_ID_DONE) {
          // Robot declares itself done
          // If this is a new report, check again if all robots done
          if (robots_done[data] == false) {
            robots_done[data] = true;
            all_done = check_all_done(robots_done);
            #ifdef DEBUG
            sprintf(buf, "[%d] \"Done\" from [%d]", my_id, data);
            Serial.println(buf);
            if (all_done) {
              sprintf(buf, "[%d] --- All Done ---", my_id);
              Serial.println(buf);
            }
            #endif
          }
        }
        
        else {
          // Data received from a robot
          // If this is new data, check again if all data has been received
          if (robots_data[robot_id] == -1) {
            robots_data[robot_id] = data;
            all_data = check_all_data(robots_data);
            #ifdef DEBUG
            sprintf(buf, "[%d] Data from [%d]: %d", my_id, robot_id, data);
            Serial.println(buf);
            if (all_data) {
              sprintf(buf, "[%d] --- All Data ---", my_id);
              Serial.println(buf);
            }
            #endif
          }
        }
      }
    }
  }

  // Tabulate score
  // Count number of aligned bits in Chaser 1 and Beater 1 (or 2) data
  // 10 points for each aligned bit, 150 if the snitch is found
  int beater_chaser_1 = robots_data[ROBOT_ID_CHASER1] & robots_data[ROBOT_ID_BEATER1];
  int beater_chaser_2 = robots_data[ROBOT_ID_CHASER2] & robots_data[ROBOT_ID_BEATER2];
  int seeker = !!robots_data[ROBOT_ID_SEEKER]; // cast to 0 or 1
  return __builtin_popcount(beater_chaser_1) * 10 + __builtin_popcount(beater_chaser_2) * 10 + seeker * 150;
}
