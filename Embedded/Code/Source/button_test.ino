

int led = D5; // LED is connected to D0
int button = D6; // Push button is connected to D2
int speaker = A1;
// This routine runs only once upon reset
void setup() 
{
  pinMode(led, OUTPUT); // Initialize D0 pin as output
  pinMode(button, INPUT_PULLUP); 
  pinMode(speaker, OUTPUT);
  // Initialize D2 pin as input with an internal pull-up resistor
}

// This routine loops forever
void loop() 
{
    //-- take control of the LED
RGB.control(true);

//-- resume normal operation
//RGB.control(false);

// red, green, blue, 0-255
RGB.color(0, 0, 0);
  int pushButtonState; 

  pushButtonState = digitalRead(button);

  if(pushButtonState == LOW)
  { // If we push down on the push button
    digitalWrite(led, HIGH);  // Turn ON the LED
    RGB.color(0, 0, 255);
    int i = 0;
    int j = 1200;
    while(pushButtonState == LOW){
        
        buzz(j);
        pushButtonState = digitalRead(button);
        i++;
        if(i <= 500){
            j--;
        }
        if(i >=500){
            j++;
        }
        if(i == 1000){
            i = 0;
            }
    }

  }
  else
  {
    digitalWrite(led, LOW);   // Turn OFF the LED 
    RGB.color(255, 0, 0);

  }

}

void buzz(int freq){
    

        digitalWrite(speaker, HIGH);
        microDelay(freq);
        digitalWrite(speaker, LOW);
        microDelay(freq);
    
}

void microDelay(int del){
  int micro = micros();
  while(1){
  if(micros() - micro >= del){
    //--do something every 5 seconds here
    break;
  }
}
    
}

