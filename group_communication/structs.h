#ifndef STRUCTS_H_
#define STRUCTS_H_

struct ScoringData {

  // Robot id
  int my_id;
  
  // Message to broadcast to other robots
  char my_message;

  // Data from other robots
  int robots_data[5];

  // Robots which have confirmed reception of data
  boolean robots_confirmed[5];

  // Robots which have broadcast "done"
  boolean robots_done[5];
  
};

ScoringData sd_dummy;

#endif
