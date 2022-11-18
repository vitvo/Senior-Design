// 28BYJ-48 Motor is operated using full step (11.25o each step) and there are 2038 steps/rev

// Define the IN pins (representing the coils) from the connecting 
// ULN2003 to the digital inputs of Arduino 
#define STEPPER_PIN_1 8
#define STEPPER_PIN_2 9
#define STEPPER_PIN_3 10
#define STEPPER_PIN_4 11

int step_number = 0;
int steps_per_rev = 2038;
int delay_time_per_slice = 2000;
int plunger_dist = 0;

// Check each step
bool complete_slice = false; // complete slices of a sample
bool complete_return_home = false; // return the plunger to original position

void setup() {
  // Initialize serial communication at 9600bits/s
  Serial.begin(9600);
    
  // Define the output to coils of stepper motor
  pinMode(STEPPER_PIN_1, OUTPUT);
  pinMode(STEPPER_PIN_2, OUTPUT);
  pinMode(STEPPER_PIN_3, OUTPUT);
  pinMode(STEPPER_PIN_4, OUTPUT);
}

void loop() {
  // Slice agarose-embedded biopsy to get technical replicates
  sliceSample(1);

  // Return the plunger to original position
  returnHome();
}

void returnHome(){
  moveStepperNut(plunger_dist, false)
  complete_return_home = true;  
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
void sliceSample(float volume){
  // volume -> mL or cc
  // Insulin syringe (Product code McMaster: 7510A603, ID: 0.18"). The volume of the cylinder inside is approximately
  // pi*r^2*h with h as the unknown (translate volume to distance) --> h = V/(pi*r^2) 

  float slice_thickness = 0.3; //300mu-m

  float radius = (0.18 / 2 * 2.54); // Convert 0.18" to cm
  float syringe_height = volume / (PI * (radius*radius)); //volume in mL/cm^3 to mm^3
  
  int slice_cut = slice_thickness;
  while(slice_cut < syringe_height){
    moveStepperNut(slice_thickness, true);
    delay(delay_time_per_slice);
    slice_cut+= slice_thickness;
  }
  plunger_dist = slice_cut;

  // Complete step check
  complete_slice = true;
}

void moveStepperNut(float distance, bool dir){
  // Tr8x2 lead nut --> 2mm per revolution, each biopsy slice must be cut 300mm or 0.3mm
  // 0.3/2mm = 0.15 of a full revolution --> 0.15(2038 steps) = 305.7 (round to 306mm steps)
  int lin_dist_per_rev = 2; //mm per revolution (Tr8x2 lead nut) --> 
  int steps_num = round((distance / lin_dist_per_rev) * steps_per_rev); //(mm/(mm/rev))*(steps/rev) = rev
  
  if(!complete_slice){
    int current_revs_num = 0;
    while(current_revs_num < steps_num){
      OneStep(dir);
      delay(2);
      current_revs_num++;
    }
  }
}

void OneStep(bool dir){
  // OneStep function to rotate the stepper motor one step with the
  // Boolean defining the direction of the rotation 

    if(dir){
      // TRUE = CW 
      switch(step_number){
      case 0:
      digitalWrite(STEPPER_PIN_1, HIGH);
      digitalWrite(STEPPER_PIN_2, LOW);
      digitalWrite(STEPPER_PIN_3, LOW);
      digitalWrite(STEPPER_PIN_4, LOW);
      break;
  
      case 1:
      digitalWrite(STEPPER_PIN_1, LOW);
      digitalWrite(STEPPER_PIN_2, HIGH);
      digitalWrite(STEPPER_PIN_3, LOW);
      digitalWrite(STEPPER_PIN_4, LOW);
      break;
      
      case 2:
      digitalWrite(STEPPER_PIN_1, LOW);
      digitalWrite(STEPPER_PIN_2, LOW);
      digitalWrite(STEPPER_PIN_3, HIGH);
      digitalWrite(STEPPER_PIN_4, LOW);
      break;
      
      case 3:
      digitalWrite(STEPPER_PIN_1, LOW);
      digitalWrite(STEPPER_PIN_2, LOW);
      digitalWrite(STEPPER_PIN_3, LOW);
      digitalWrite(STEPPER_PIN_4, HIGH);
      break;
    } 
    }else{
      switch(step_number){
      case 0:
      digitalWrite(STEPPER_PIN_1, LOW);
      digitalWrite(STEPPER_PIN_2, LOW);
      digitalWrite(STEPPER_PIN_3, LOW);
      digitalWrite(STEPPER_PIN_4, HIGH);
      break;

      case 1:
      digitalWrite(STEPPER_PIN_1, LOW);
      digitalWrite(STEPPER_PIN_2, LOW);
      digitalWrite(STEPPER_PIN_3, HIGH);
      digitalWrite(STEPPER_PIN_4, LOW);
      break;

      case 2:
      digitalWrite(STEPPER_PIN_1, LOW);
      digitalWrite(STEPPER_PIN_2, HIGH);
      digitalWrite(STEPPER_PIN_3, LOW);
      digitalWrite(STEPPER_PIN_4, LOW);
      break;
      
      case 3:
      digitalWrite(STEPPER_PIN_1, HIGH);
      digitalWrite(STEPPER_PIN_2, LOW);
      digitalWrite(STEPPER_PIN_3, LOW);
      digitalWrite(STEPPER_PIN_4, LOW);
      }
    }
step_number++;
  if(step_number > 3){
    step_number = 0;
  }
}
