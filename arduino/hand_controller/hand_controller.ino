/*$Id: hand_controller.ino | Sun Jul 29 17:38:52 2018 -0500 | cytan  $*/
/*
    hand_controller is the controller code for the Arduino Sparkfun Pro Micro
    Copyright (C) 2018  C.Y. Tan
    Contact: cytan299@yahoo.com

    This file is part of hand_controller

    hand_controller is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    hand_controller is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with hand_controller.  If not, see <http://www.gnu.org/licenses/>.

*/

/* operating system header files (use <> for make depend) */
#include <Arduino.h>
#include <string.h>

/* general system header files (use "" for make depend) */

#include <SPI.h>  
#include <SFE_MicroOLED.h>
#include <MenuSystem.h>

/* local include files (use "") */

/**********************************************************************
  There are two hardware versions. The development version is Version 1
  and the release version is version 2.
  So for all users: uncomment VERSION2 if it is commented out below
 **********************************************************************/

//#define VERSION2

/**********************************************************************
  Define the pins of the OLED and make the OLED
 **********************************************************************/
#define OLED_PIN_RESET A1  
#define OLED_PIN_DC    A0  
#define OLED_PIN_CS    A2 

MicroOLED oled(OLED_PIN_RESET, OLED_PIN_DC, OLED_PIN_CS); // SPI declaration

// size of the oled
#define PIXEL_XMAX	64
#define PIXEL_YMAX	48

/**********************************************************************
  Define the pins used for the joystick and its button
 **********************************************************************/
#define JOY_RA		A8
#define JOY_DEC		A9
#define JOY_BUTTON	A10

/**********************************************************************
  Define the led lights pwm pin
 **********************************************************************/
#define LED_LIGHT	5

/**********************************************************************
  Define the menu and back buttons
 **********************************************************************/
#define BACK_BUTTON	6
#define MENU_BUTTON	7

/**********************************************************************
NAME

	AP - class that encapsulates the commands to control the AP mount
		   
SYNOPSIS
	
	AP is the class that encapsulates the commands to control the
	AP mount using serial commands given in the protocol-G.pdf
	manual. Only the necessary commands for this handcontroller
	are implemented.

INTERFACE
	ClearBuffer()	- Tell GTOCP4 to clear its buffer
	IsConnected()	- Returns true if the handcontroller is
		          connected to the GTOCP4 and a valid version
			  is returned. Doesn't work with control boxes
			  with software versions earlier than B.
	GetVersion(	- Get the version of the GTOCP4
		v	- version string sent back here. v is an int array.
	)		- returns false if no version string is
			  returned
	SendSlewRate(	- send the slew rate to the GTOCP4
		v	- to this value. Note: NO CHECKS are done at
			  this time. 
	)
	IncrementSlewRate() - increment the slew rate according to the
			      values in _SLEW_RATES[]. Slew rate will
			      wrap around when it gets to the end of
	                      the _SLEW_RATES[] array.
			      Returns the value of the slew rate.
			      NOTE: Nothing gets sent to the GTOCP4
			      until SendSlewRate() is called.
			      
	DecrementSlewRate() - decrement the slew rate according to the
			      values in _SLEW_RATES[]. Slew rate will
			      wrap around when it gets to the start of
	                      the _SLEW_RATES[] array.
			      Returns the value of the slew rate.
			      NOTE: Nothing gets sent to the GTOCP4
			      until SendSlewRate() is called.

	SetSlewRateI(	- Set the slew rate to this _SLEW_RATES[]	
		i	- index value	
	)		- NO CHECKS ARE DONE! and nothing getssent
		          to the GTOCP4 until SendSlewRate() is called.
	GetSlewRate()	- returns the current slew rate.
	GetSlewRateI()	- returns the current slew rate index of
			  _SLEW_RATES[]
	SetAndSendSlewRateI( -Set the slew rate to this _SLEW_RATES[]	
		i	     - index value and send it to the GTOCP4.	
	)
	GetSlewRateName(  - return the name of the slew rate	
		i	  - for this index in _SLEW_RATE_NAMES[]
	)
	Move(		- Move the telescope in the direction
	  d		- given by d
	)		- Slew rate has been set previously with the
			  *SlewRate*() functions.
	GetDirection()	- return the direction that the telescope is
			  slewing.
	SetHome()	- set the home position of the telescope.
			  Returns 0 on success
				 -1: cannot set ALT
				 -2: cannot set AZ
	GoHome()	- Tell telescope to go to previously set HOME
			  position.
			  Returns 0 on success
				 -1: send ALT failed
				 -2: send AZ failed
				 -3: GTOCP4 go home failed
				 -4: No HOME value set

	IsHomeSet()	- returns true if HOME has been set
	IsHome()	- returns true if the telescope is at HOME
			  position.
	Park()		- park the telescope at this current position.


PRIVATE FUNCTIONS
	send(		- send
	   cmd		  this command string to the GTOCP4	
	)		- returns 0 on success

	receive(	- receive the 
	   data		  data from the GTOCP4
	   stop		  The stop character. Default: "#"
	)		- returns the size of the data array.
			  -1 on failure. receive() does not wait
			  forever. It will fail after some time and
			  returns -1.
	
AUTHOR
	C.Y. Tan

SEE ALSO
	protocol-G.pdf

**********************************************************************/

enum DIRECTION {NORTH, NORTH_EAST, NORTH_WEST,
		SOUTH, SOUTH_EAST, SOUTH_WEST,
		EAST, WEST,
		STOP, FORCE_STOP};

class AP
{
public:
  AP();
public:
  void ClearBuffer();
  bool IsConnected();
  bool GetVersion(int* v);

  void SendSlewRate(const int s);
  int IncrementSlewRate();
  int DecrementSlewRate();
  void SetSlewRateI(const int i);
  int GetSlewRate() const;
  int GetSlewRateI() const;
  void SetAndSendSlewRateI(const int i);
  char* GetSlewRateName() const;
  void Move(DIRECTION d);
  DIRECTION GetDirection() const;
  int SetHome();
  int GoHome();
  bool IsHomeSet();
  bool IsHome();
  void Park();

public:
  static char* _SLEW_RATE_NAMES[];  
  
private:
  int send(const __FlashStringHelper*cmd) const;
  int receive(int* const data, int stop='#') const;

  int _slew_rate_i;
  int _max_slew_rate_i;
  static int _SLEW_RATES[];

  DIRECTION _dir;

  bool _is_home_set;
  char _alt[32];
  char _az[32];

  
};

/*
    _SLEW_RATES[] values:
	0: 12x
	1: 64x
	2: 600x
	3: 1200x
*/
int AP::_SLEW_RATES[] = {0, 1, 2, 3};

char* AP::_SLEW_RATE_NAMES[] = {"Fine_12x",
				"Slow_64x",
				"Med_600x",
				"Fast_1200x"};

AP::AP()
{
  _slew_rate_i = 2; // default slew rate is 600x
  _max_slew_rate_i = 4;//sizeof(_SLEW_RATES)/sizeof(int);
  _dir = STOP;

  _is_home_set = false;
  _alt[0] ='\0';
  _az[0] = '\0';
}
void AP::ClearBuffer()
{
  send(F("##"));
}

bool AP::IsConnected()
{
  if(Serial1){
    //check version of the control box
    int buf[16];
    return GetVersion(buf);
  }
  else {
    return false;
  }
}

bool AP::GetVersion(int* v)
{
  send(F("#:V#"));
  int len = receive(v);
  if(len > 0){
    if(v[len-1] == '#'){
      return true;
    }
    else return false;
  }
  else return false;
}

void AP::SendSlewRate(const int v)
{
  // Use centering rate :RCxxx# rather than slew rate :RSxxx#
  // The reason is that :RCxxx# is what the handcontroller will use
  // while the :RSxxx# will be used by the ASCOM driver.
  if(Serial1){
    Serial1.print("#:RC");
    Serial1.print(v, DEC);
    Serial1.println("#");
    Serial1.flush();
  }
}

int AP::IncrementSlewRate()
{
  _slew_rate_i++;
  if(_slew_rate_i >= _max_slew_rate_i){
    // wrap around
    _slew_rate_i = 0;
  }
  return _SLEW_RATES[_slew_rate_i];
}

int AP::DecrementSlewRate()
{
  _slew_rate_i--;
  if(_slew_rate_i < 0){
    // wrap around
    _slew_rate_i = _max_slew_rate_i-1;
  }
  return _SLEW_RATES[_slew_rate_i];
}

void AP::SetSlewRateI(const int i)
{
  _slew_rate_i = i;
}

int AP::GetSlewRate() const
{
  return _SLEW_RATES[_slew_rate_i];
}

int AP::GetSlewRateI() const
{
  return _slew_rate_i;
}

void AP::SetAndSendSlewRateI(const int i)
{
  _slew_rate_i = i;
  SendSlewRate(_SLEW_RATES[_slew_rate_i]);
}

char* AP::GetSlewRateName() const
{
  return _SLEW_RATE_NAMES[_slew_rate_i];
}
   

void AP::Move(DIRECTION d){

  if(d == FORCE_STOP){
    send(F("#:Q#")); // FORCE a STOP!!!!
    _dir=FORCE_STOP;

    return;
  }
  
  // move until stop is issued.
  switch (d) {
    case NORTH:
      if(_dir != NORTH){
	send(F("#:Q#")); // stop the previous direction
        send(F("#:Mn#"));
	_dir = NORTH;
      } // don't do anything if we are already moving north
      break;
    case SOUTH:
      if(_dir != SOUTH){
        send(F("#:Q#")); // stop the previous direction	
        send(F("#:Ms#"));
	_dir = SOUTH;
      }
      break;
    case EAST :
      if(_dir != EAST){
        send(F("#:Q#")); // stop the previous direction
        send(F("#:Me#"));
	_dir = EAST;
      }
      break;
    case WEST :
      if(_dir != WEST){
	send(F("#:Q#")); // stop the previous direction
        send(F("#:Mw#"));
	_dir = WEST;
      }
      break;
    case NORTH_EAST:
      if(_dir != NORTH_EAST){
	send(F("#:Q#")); // stop the previous direction
        send(F("#:Mn#"));
        send(F("#:Me#"));	
	_dir = NORTH_EAST;
      } 
      break;
    case NORTH_WEST:
      if(_dir != NORTH_WEST){
	send(F("#:Q#")); // stop the previous direction
        send(F("#:Mn#"));
        send(F("#:Mw#"));	
	_dir = NORTH_WEST;
      } 
      break;
    case SOUTH_EAST:
      if(_dir != SOUTH_EAST){
	send(F("#:Q#")); // stop the previous direction
        send(F("#:Ms#"));
        send(F("#:Me#"));	
	_dir = SOUTH_EAST;
      } 
      break;
    case SOUTH_WEST:
      if(_dir != SOUTH_WEST){
	send(F("#:Q#")); // stop the previous direction
        send(F("#:Ms#"));
        send(F("#:Mw#"));	
	_dir = SOUTH_WEST;
      } 
      break;            
    case STOP :
      if(_dir != STOP){
	send(F("#:Q#")); // stop moving!
	_dir = STOP;
      }
      break;
  }
}

DIRECTION AP::GetDirection() const
{
  return _dir;
}

int AP::SetHome()
{
  int data[32];
  int len;

  _is_home_set = false;
  
  send(F("#:GA#")); // get alt
  if((len=receive(data)) > 0){
    for(int i=0; i<len; i++){
      _alt[i] = (char)data[i];
    }
    _alt[len] = '\0';    
  }
  else {
    return -1;
  }

  send(F("#:GZ#")); // get az
  if((len=receive(data)) > 0){
    for(int i=0; i<len; i++){
      _az[i] = (char)data[i];
    }
    _az[len] = '\0';
  }
  else {
    return -2;
  }
#ifdef AAAAA
  Serial.print("alt"); Serial.println(_alt);
  Serial.print("az"); Serial.println(_az);
#endif
  _is_home_set = true;
  return 0;  
}

int AP::GoHome()
{
  if(_is_home_set){
    int data[32];
  
    //  :Sa sDD*MM# or :Sa sDD*MM:SS#
    
    if(Serial1){
      // send alt
      Serial1.print(F("#:Sa "));
      Serial1.println(_alt); // alt already contains a '#' at the end
      Serial1.flush();    
      // wait 1
      if(receive(data, '1') > 0){
	if(data[0] != '1'){
	  return -1;
	}
      }
      else {
        return -1;
      }
  

      //:Sz DDD*MM# or :Sz DDD*MM:SS#    
      Serial1.print(F("#:Sz "));
      Serial1.println(_az); // az already contains a '#' at the end
      Serial1.flush();

      // wait 1
      if(receive(data, '1') > 0){
        if(data[0] != '1'){
          return -2;
        }
      }
      else {
        return -2;
      }

      // go home
      send(F("#:MS#"));
      if(receive(data, '0') > 0){
	if(data[0] != '0'){
          return -3;
	}
      }
    } // Serial1
  } // is_home_set
  else {
    return -4;
  }
  return 0;  
}

bool AP::IsHomeSet()
{
  return _is_home_set;
}

bool AP::IsHome()
{
  int data[32];
  int len;
  
  send(F("#:GA#")); // get alt
  if((len=receive(data)) > 0){
    if(strlen(_alt) == len){
      for(int i=0; i<len; i++){
        if(_alt[i] != (char)data[i]){
	  return false;
        }
      }
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }

  send(F("#:GZ#")); // get az
  if((len=receive(data)) > 0){
    if(strlen(_az) == len){
      for(int i=0; i<len; i++){
	if(_az[i] != (char)data[i]){
	  return false;
	}
      }
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }

  return true;    
}

void AP::Park()
{
  send(F("#:KA#"));
}

int AP::send(const __FlashStringHelper*cmd) const
{
  if(Serial1){
    Serial1.println(cmd);
    Serial1.flush();
  }
  else {
    return -1;
  }
  return 0;
}
  
int AP::receive(int* const data, int stop) const
{
  if(!Serial1)
    return -1;
    
  int* pdata = data;
  int len = 0;
  const long wait_limit = 1000000;
  long wait_i = 0;
#ifdef AAAAAAA
    Serial.print("Received: ");
#endif
  bool is_done = false;
  while(!is_done && wait_i < wait_limit){
  int num_bytes;
  if((num_bytes=Serial1.available()) > 0){
    wait_i = 0; // reset wait
      
    len += num_bytes;
    for(int i=0; i<num_bytes; i++){
      int ch = Serial1.read();
      *pdata++ = ch;
      // write the char
#ifdef AAAAAA	
	  Serial.write(ch);
#endif	
      if(ch == stop)
	is_done = true;
      }
    }
    else {
      wait_i++;
    }
  }
#ifdef AAAAAA    
    Serial.println("");
    Serial.print("wait_i=");
    Serial.println(wait_i, DEC);
#endif  
  return wait_i >= wait_limit? -1: len;    
}

AP ap;

/**********************************************************************
NAME

	JoyStick - class that encapsulates the behaviour of the joystick
		   
SYNOPSIS
	
	JoyStick allows the user to query the joystick as a 4 position
	switch or to return its value that is mapped from -512 to
	+512.

INTERFACE
	
	GetDecState()	- returns JOY_UP, JOY_DOWN or JOY_CENTRED
	GetRAState()	- returns JOY_LEFT, JOY_RIGHT or JOY_CENTRED
	IsClicked()	- returns true if the joystick button is
			  clicked
	GetR()		- returns the radial position of the
			  joystick. 
	
AUTHOR
	C.Y. Tan

SEE ALSO

**********************************************************************/
enum JOY_STATE { JOY_CENTRED, JOY_DOWN, JOY_UP, JOY_LEFT, JOY_RIGHT};

#define DEBOUNCE_TIME	100 // ms
class JoyStick {
public:
  JoyStick(){
    // set up joystick button
    pinMode(JOY_BUTTON, INPUT);
    digitalWrite(JOY_BUTTON, HIGH);
  }
  
  JOY_STATE GetDecState() const{
    // wait for a while before reading state
    delay(DEBOUNCE_TIME);
    // read the joystick dec
    int dec = analogRead(JOY_DEC);
    // 500 in middle. Up is 0, down is 1024    
    if(dec < 480){
      return JOY_UP;
    }

    if(dec > 544){
      return JOY_DOWN;
    }
    return JOY_CENTRED;
  }

  JOY_STATE GetRAState() const{
    // wait for a while before reading state
    delay(DEBOUNCE_TIME);    
    // read the joystick ra
    int ra = analogRead(JOY_RA);
    // 500 in middle. right is 0, left is 1024    
    if(ra < 480){
      return JOY_RIGHT;
    }

    if(ra > 544){
      return JOY_LEFT;
    }
    return JOY_CENTRED;    
  }

  bool IsClicked() const{
    // wait for a while before reading state    
    delay(DEBOUNCE_TIME);
    return digitalRead(JOY_BUTTON) == 0;
  }

  int GetR() const{
    float ra = static_cast<float>(analogRead(JOY_RA) - 512);
    float dec = static_cast<float>(analogRead(JOY_DEC) - 512);
#ifdef AAAAA
    Serial.print("("); Serial.print(ra, DEC); Serial.print(",");
    Serial.print(dec, DEC); Serial.println(")");
#endif
    return static_cast<int>(sqrt(ra*ra + dec*dec));
  }

};

JoyStick joy_stick;

/**********************************************************************
NAME

	MenuBackButtons - class that encapsulates the behaviour of the
	two buttons that the user can push
		   
SYNOPSIS
	
	MenuBackButtons is a class that encapsulates the behaviour of
	the two buttons that the user can push. They are the MENU
	button and the BACK button

INTEFACE
	GetState() - returns MENU_ON, BACK_ON or BOTH_OFF	
	
AUTHOR
	C.Y. Tan

SEE ALSO

**********************************************************************/

enum BUTTON_STATE {MENU_ON, BACK_ON, BOTH_OFF};

class MenuBackButtons {
public:
  MenuBackButtons(){
    //set up the menu and back buttons
    pinMode(MENU_BUTTON, INPUT);
    digitalWrite(MENU_BUTTON, HIGH);

    pinMode(BACK_BUTTON, INPUT);
    digitalWrite(BACK_BUTTON, HIGH);      
  }
  
  BUTTON_STATE GetState() const {
    delay(DEBOUNCE_TIME);
    if(digitalRead(MENU_BUTTON) == 0){
      return MENU_ON;
    }

    if(digitalRead(BACK_BUTTON) == 0){
      return BACK_ON;
    }

    return BOTH_OFF;
  }
};

MenuBackButtons mb_buttons;


/**********************************************************************
NAME

	LEDLight - class that controls the brightness of the LED lights

		   
SYNOPSIS
	
	LEDLight class controls the brightness of the LED lights

INTERFACE
	Increase()	- increase the brightness of the LED lights
	Decrease()	- decrease the brightness of the LED lights
	
AUTHOR
	C.Y. Tan

SEE ALSO

**********************************************************************/

class LEDLight
{
public:
  LEDLight(){
    // setup PWM for the led light
    pinMode(LED_LIGHT, OUTPUT);
    _level = 0x80;
    analogWrite(LED_LIGHT, _level);
  }

  void Increase(){
    if(_level+4 < 255){
      _level+= 4;
    }
    else {
      _level = 255;
    }
    analogWrite(LED_LIGHT, _level);
  }

  void Decrease(){
    unsigned char level = _level;
    if(_level - 4 > 0){
      _level -= 4;
    }
    else {
      _level = 0;
    }
    analogWrite(LED_LIGHT, _level);    
  }

  unsigned char GetLevel() const
  {
    return _level;
  }

private:
  int _level;
};

LEDLight led;

/**********************************************************************
NAME

	HCRenderer - the handcontroller OLED renderer for menu items
		   
SYNOPSIS
	
	HCRenderer is the class for rendering menu items of the
	OLED. This is a required class for the MenuSystem

INTERFACE
	The following functions have to be defined so that the base
	class MenuComponentRenderer's virtual functions
	render()	- defines how the menu is rendered on the OLED

	render_menu_item(), render_back_menu_item(),
	render_numeric_menu_item() and render_menu() are all functions
	that render the menu on the OLED display

	
AUTHOR
	C.Y. Tan

SEE ALSO

**********************************************************************/

// font 0 is 5x7
#define FONT_H 7 // font height
#define LINE_SPACE 4 // spacing between lines
#define SELECT_SPACE 2 // spacing below the font for select line

class HCRenderer: public MenuComponentRenderer{
public:
  void render(Menu const& menu) const {
    oled.clear(PAGE); // clear the buffer
    oled.setCursor(0,0);
    oled.print(menu.get_name());
    oled.display();	    

    for(int i=0; i<menu.get_num_components(); i++){
      MenuComponent const* cp = menu.get_menu_component(i);
      
      oled.setCursor(0, (i+1)*(FONT_H+LINE_SPACE));
      
      if(cp != NULL){
	cp->render(*this);
	if(cp->is_current()){
	  oled.line(0, (i+1)*(FONT_H+LINE_SPACE) + (FONT_H+SELECT_SPACE),
		    PIXEL_XMAX, (i+1)*(FONT_H+LINE_SPACE) + (FONT_H+SELECT_SPACE));	  
	  oled.display();
	}
      }
      else {
      }
    }
  }

  void render_menu_item(MenuItem const& menu_item) const {
    oled.print(menu_item.get_name());
    oled.display();    
  }

  void render_back_menu_item(BackMenuItem const& menu_item) const {
    oled.print(menu_item.get_name());
    oled.display();    
  }

  void render_numeric_menu_item(NumericMenuItem const& menu_item) const {
    oled.print(menu_item.get_name());
    oled.display();    
  }

  void render_menu(Menu const& menu) const {
    oled.print(menu.get_name());
    oled.display();    
  }
};

/**********************************************************************
  Define the menu items
 **********************************************************************/

HCRenderer hc_renderer;

// Forward declarations

void on_nav_selected(MenuComponent*);
void on_rate_selected(MenuComponent*);
void on_ra_move_selected(MenuComponent*);
void on_prop_nav_selected(MenuComponent*);
void on_sethome_selected(MenuComponent*);
void on_gohome_selected(MenuComponent*);
void on_led_selected (MenuComponent*);

// Menu variables

MenuSystem ms(hc_renderer);
  Menu mn_nav("Navigation");
    MenuItem mi_nav("Navigate", &on_nav_selected);
    MenuItem mi_rate(ap.GetSlewRateName(), &on_rate_selected);
    MenuItem mi_prop_nav("Prop Nav", &on_prop_nav_selected);
  Menu mn_pm("PoleMaster");
    MenuItem mi_sethome("Set home", &on_sethome_selected);
    MenuItem mi_ra("RA move", &on_ra_move_selected);
    MenuItem mi_gohome("Go home", &on_gohome_selected);
  MenuItem mi_led("LED lights", &on_led_selected);



/**********************************************************************
TITLE

  The menu callbacks and helper functions

CALLBACKS
	ALL these functions depend on these globally defined variables:
		ap, oled and led

	on_nav_selected(	- The user has selected moving the
				  with the joystick
		p_menu_component
	)

	on_rate_selected(	- The user wants to change the slew
				  rate. Slew rate is changed with the joystick.
		p_menu_component
	)

	on_prop_nav_selected(	- The user wants to move the telescope
				  using digital proportional
				  control. This is an EXPERIMENTAL FEATURE!
		p_menu_component
	)

	on_ra_move_selected(	- The user wants to move RA ONLY at
				  full slew speed. This is useful for
				  PoleMaster calibration.
		p_menu_component
	)

	on_sethome_selected(	- The user wants to set the HOME
				  position of the telescope.
		p_menu_component
	)

	on_gohome_selected(	- The user wants the telescope to go
				  back HOME. Of course, HOME must have
				  been defined by the user.
		p_menu_component
	)

	on_led_selected(	- The user wants to change the
				  brightness of the led light	
		p_menu_component
	)


HELPER FUNCTIONS
	draw_direction() - draw the direction that the joystick
			   is pointing

	navigation(	 - figure out which direction that joystick is pointing
	  dir_ns, dir_ew - return the direction in these variables
	)	         - returns -1 if either the BACK or MENU buttons are
			   pushed to tell the user to get out of
			   navigation


	process_menu()	- This function lives in the loop()
			  function. It sits forever waiting for user
			  input via the position of the joystick. A
			  cursor is drawn below the menu item to show
			  what the user is selecting. Here are the
			  possible user actions:
				(1) Swiping the joystick to the LEFT usually gets back to
				    the previous menu.
				(2) SWIPING to the right selects the
				    current menu item.
				(3) Clicking the joystick button
				    selects the current menu item.
				(4) Using the BACK button goes to
				    previous menu. Slews are aborted.
				(5) Using MENU button sends the user
				    back to the main menu. Slews are aborted.

 **********************************************************************/

void draw_direction()
{
  DIRECTION dir = ap.GetDirection();
  oled.clear(PAGE);
  oled.setFontType(1); // 8x16 font  
  oled.circle(32,24, 23);
  switch(dir){
    case NORTH:
      oled.setCursor(30,0);
      oled.print("N");
    break;
    case SOUTH:
      oled.setCursor(30,36);
      oled.print("S");
    break;
    case EAST:
      oled.setCursor(9,19);      
      oled.print("E");
    break;
    case WEST:
      oled.setCursor(48,19);
      oled.print("W");
    break;
    case NORTH_EAST:
      oled.setCursor(12,3);
      oled.print("NE");
    break;
    case NORTH_WEST:
      oled.setCursor(46,3);      
      oled.print("NW");
    break;
    case SOUTH_EAST:
      oled.setCursor(12,35);
      oled.print("SE");
    break;
    case SOUTH_WEST:

      oled.setCursor(46,35);      
      oled.print("SW");
    break;
    case STOP:
      oled.setCursor(30,19);//centre      
      oled.print("0");
    break;
  }

  if(dir != STOP){
    oled.setCursor(30,19);
    oled.print(ap.GetSlewRateI()+1);
  }

  oled.setFontType(0);    
  // print the slew rate on the corner
  oled.setCursor(5, 40);
  oled.print(ap.GetSlewRateI()+1);
  mi_rate.set_name(ap.GetSlewRateName());  
  
  oled.display();

}

int navigation(DIRECTION* dir_ns, DIRECTION* dir_ew)
{
  // check DEC first
  switch(joy_stick.GetDecState()){
    case JOY_UP:
      *dir_ns = NORTH;
    break;
    case JOY_DOWN:
      *dir_ns = SOUTH;
    break;
    case JOY_CENTRED:
      *dir_ns = STOP;
    break;
  }

  // check RA next
  switch(joy_stick.GetRAState()){
    case JOY_LEFT:
      *dir_ew = EAST;
    break;
    case JOY_RIGHT:
      *dir_ew = WEST;
    break;
    case JOY_CENTRED:
      *dir_ew = STOP;
    break;
  }

  // Now we can move the mount 
  if(*dir_ns != STOP){
    if(*dir_ew == STOP){ // only move either N or S
      switch(*dir_ns){
        case NORTH:
	  ap.Move(NORTH);
	break;
	case SOUTH:
	  ap.Move(SOUTH);
	break;
      }
    }
    else { // NE, NW, SE, SW
      switch(*dir_ns){
        case NORTH:
	  switch(*dir_ew){
	    case EAST:
	      ap.Move(NORTH_EAST);
	    break;
	    case WEST:
	      ap.Move(NORTH_WEST);
	    break;		
	  } // north
	  break;
	case SOUTH:
	  switch(*dir_ew){
	    case EAST:
	      ap.Move(SOUTH_EAST);
	    break;
	    case WEST:
	      ap.Move(SOUTH_WEST);
	    break;
	  } // south
	  break;
      } // switch dir_ns
    } // else NE, NW, SE, SW
  } // dir_ns != STOP
  else { // dir_ns == STOP
    switch(*dir_ew){
      case EAST:
	ap.Move(EAST);
      break;
      case WEST:
        ap.Move(WEST);
      break;
      case STOP:
        ap.Move(STOP);
      break;
    } // switch dir_ew
  }

  // print out the direction on the oled 
  draw_direction();
    
  if(mb_buttons.GetState() == BACK_ON){
    // make sure we stop the mount first!
    ap.Move(FORCE_STOP);
    return -1; // get out	      
  }

  if(mb_buttons.GetState() == MENU_ON){
    // make sure we stop the mount first!
    ap.Move(FORCE_STOP);
    ms.reset(); // reset the menu
    return -1; // get out
  }

  return 0;
}

void on_nav_selected(MenuComponent* p_menu_component){
  DIRECTION dir_ns = STOP, dir_ew = STOP;
  
  while(navigation(&dir_ns, &dir_ew) == 0){
    int new_slew_rate;
    if(joy_stick.IsClicked()){
      new_slew_rate=ap.IncrementSlewRate();
      ap.SendSlewRate(new_slew_rate);
    }
  }

}

void on_rate_selected(MenuComponent* p_menu_component){
  oled.clear(PAGE);
  oled.setCursor(0,0);
  oled.print("Slew rate:");
  oled.setCursor(0, FONT_H+LINE_SPACE);

  int old_slew_rate = ap.GetSlewRate();
  int old_slew_rate_i = ap.GetSlewRateI();
  oled.print(old_slew_rate, DEC); oled.print("x");
  oled.display();

  // Use joystick to increase or decrease slew rates
  int new_slew_rate = old_slew_rate;
  while(1){
    switch(joy_stick.GetDecState()){
      case JOY_UP:
        new_slew_rate = ap.IncrementSlewRate();
      break;
      case JOY_DOWN:
        new_slew_rate = ap.DecrementSlewRate();
      break;
    }

    oled.setCursor(0, FONT_H+LINE_SPACE);
    oled.print(AP::_SLEW_RATE_NAMES[new_slew_rate]); oled.print("  ");
    oled.display();
    
    if(joy_stick.IsClicked()){
      ap.SendSlewRate(new_slew_rate); // send the new slew rate to the mount
      mi_rate.set_name(ap.GetSlewRateName());
      break; // get out	      
    }

    // oops! Abort change of rate
    if(mb_buttons.GetState() == BACK_ON){
      ap.SetSlewRateI(old_slew_rate_i);
      break;
    }
    // oops! Abort change rate and get back to root menu
    if(mb_buttons.GetState() == MENU_ON){
      ap.SetSlewRateI(old_slew_rate_i);
      ms.reset();
      break;
    }    
  }

  oled.setFontType(0); // 5x7 font
}

void on_prop_nav_selected(MenuComponent* p_menu_component)
{
  // remember the old slew rate
  int old_slew_rate_i = ap.GetSlewRateI();

  // set slew rate to the slowest rate first
  ap.SetAndSendSlewRateI(0);
  int current_slew_rate = 0;
  int prev_slew_rate = 0;
  
  DIRECTION dir_ns = STOP, dir_ew = STOP;
  while(navigation(&dir_ns, &dir_ew) == 0){
    // check how far the joystick is from the centre
    int r = joy_stick.GetR();
#ifdef AAAA
        Serial.print("r="); Serial.println(r, DEC);
#endif
    if(r < 16){		//0
      ap.SetSlewRateI(0);
      current_slew_rate = 0;
      
      ap.Move(STOP);
    }
    else {
      if(r < 256){	// 1
	ap.SetSlewRateI(0);
        current_slew_rate = 0;	
      }
      else {
	if(r < 475){	//2
	  ap.SetSlewRateI(1);
          current_slew_rate = 1;	  
	}
	else {
	  if(r < 510){	//3
	    ap.SetSlewRateI(2);
	    current_slew_rate = 2;	    
	  }
	  else {	//4
	    ap.SetSlewRateI(3);
	    current_slew_rate = 3;	    
	  }
	}
      }
    }

    if(current_slew_rate != prev_slew_rate){
      ap.SendSlewRate(ap.GetSlewRate());
      prev_slew_rate = current_slew_rate;
    }
  }
  // restore the old rate
  ap.SetAndSendSlewRateI(old_slew_rate_i);
  ap.Move(FORCE_STOP);
}

void on_ra_move_selected(MenuComponent* p_menu_component)
{
  DIRECTION dir_ew = STOP;

  int old_slew_rate_i = ap.GetSlewRateI(); // remember the previous slew rate  
  // set fastest slew rate at 1200x which is index 3
  ap.SetAndSendSlewRateI(3);
  int slew_rate = ap.GetSlewRate();
  oled.setFontType(1); // 8x16 font
  while(1){
    switch(joy_stick.GetRAState()){
      case JOY_LEFT:
        ap.Move(EAST);
	dir_ew = EAST;
      break;
      case JOY_RIGHT:
        ap.Move(WEST);
	dir_ew = WEST;	
      break;
      case JOY_CENTRED:
        ap.Move(STOP);
	dir_ew = STOP;
      break;
    }

    // now print out what the user is doing
    draw_direction();
    
    if(mb_buttons.GetState() == BACK_ON){
      // make sure we stop the mount first!
      ap.Move(FORCE_STOP);
      // reset to old slew rate
      ap.SetAndSendSlewRateI(old_slew_rate_i);
      mi_rate.set_name(ap.GetSlewRateName());
      
      break; // get out	      
    }

    if(mb_buttons.GetState() == MENU_ON){
      // make sure we stop the mount first!
      ap.Move(FORCE_STOP);
      // reset to old slew rate      
      ap.SetAndSendSlewRateI(old_slew_rate_i);
      mi_rate.set_name(ap.GetSlewRateName());
      
      ms.reset(); // get back to the main menu
      break; // get out	      
    }    
  } // while
}

void on_sethome_selected(MenuComponent* p_menu_component)
{
  oled.clear(PAGE);
  oled.setCursor(0,10);
  oled.println("Setting...");
  oled.display();

  oled.clear(PAGE);
  oled.setCursor(0,10);  
  // read current position and assume that it is home
  if(ap.SetHome() == 0){
    oled.println("Success!");
    return; // done so go back
  }
  else {
    oled.println("FAILED!");
  }
  oled.display();

  while(1){
    if((joy_stick.GetRAState() == JOY_LEFT) ||
       mb_buttons.GetState() == BACK_ON){
      return;
    }
  }
}

void on_gohome_selected(MenuComponent* p_menu_component)
{
  int err = 0;
  
  oled.clear(PAGE);
  oled.setCursor(0,10);
  oled.println("Going ...");
  oled.display();

  oled.clear(PAGE);
  oled.setCursor(0,10);
  if((err=ap.GoHome()) >= 0){
    // wait for the mount to reach home
    oled.println("Waiting...");
    // check that the alt and az are the same
    while(1){
      if(ap.IsHome()){
	//	ap.Park();
	return;
      }
    }
  }
  else {
    switch(err){
      case -1:
	oled.println("ALT");
	oled.println("FAILED!");
      break;	
      case -2:
	oled.println("AZ");
	oled.println("FAILED!");
      break;	
      case -3:
        oled.println("Go HOME");
	oled.println("FAILED!");
      break;	
      case -4:
        oled.println("FAILED!");
	oled.println("HOME not");
	oled.println("set");
      break;	
      default:
        oled.println("FAILED!");
      break;	
    }

    oled.display();    
    while(1){
      if((joy_stick.GetRAState() == JOY_LEFT) ||
        mb_buttons.GetState() == BACK_ON){
        return;
      }
    }
  }
}

void on_led_selected(MenuComponent* p_menu_component)
{
 
  while(1){
    switch(joy_stick.GetRAState()){
      case JOY_LEFT:
        led.Decrease();
      break;
      case JOY_RIGHT:
        led.Increase();
      break;      
    }

    oled.clear(PAGE);  
    oled.setCursor(0,10);
    oled.print("LED:");
    oled.rect(0,22, 64, 10);
    // map (0 to 255) to (0 to 64)
    oled.rectFill(0,22,
		  map(led.GetLevel(), 0, 255, 0, 64),
		  10);
  
    oled.display();
  
    if((mb_buttons.GetState() == BACK_ON) ||
       (mb_buttons.GetState() == MENU_ON)){
      return;
    }
  } //while
}

void process_menu()
{
  Menu* mp = ms.get_current_menu();
  int c_i = mp->get_current_component_num();

  switch(joy_stick.GetDecState()){
    case JOY_UP:
      if(ms.prev()){
        // clear current line
        oled.line(0, (c_i+1)*(FONT_H+LINE_SPACE) + (FONT_H+SELECT_SPACE),
		PIXEL_XMAX, (c_i+1)*(FONT_H+LINE_SPACE) + (FONT_H+SELECT_SPACE), BLACK, NORM);
        // write new line which is above current one
        oled.line(0, ((c_i-1)+1)*(FONT_H+LINE_SPACE) + (FONT_H+SELECT_SPACE),
		PIXEL_XMAX, ((c_i-1)+1)*(FONT_H+LINE_SPACE) + (FONT_H+SELECT_SPACE));
        oled.display();
      }
      break;

    case JOY_DOWN:
      if(ms.next()){
        // clear current line
        oled.line(0, (c_i+1)*(FONT_H+LINE_SPACE) + (FONT_H+SELECT_SPACE),
		PIXEL_XMAX, (c_i+1)*(FONT_H+LINE_SPACE) + (FONT_H+SELECT_SPACE), BLACK, NORM);
        // write new line which is below current one      
        oled.line(0, ((c_i+1)+1)*(FONT_H+LINE_SPACE) + (FONT_H+SELECT_SPACE),
		PIXEL_XMAX, ((c_i+1)+1)*(FONT_H+LINE_SPACE) + (FONT_H+SELECT_SPACE));

        oled.display();
      }
      break;

    case JOY_CENTRED: 
      break;
  } // case

  // go back up of the menu
  
  if((joy_stick.GetRAState() == JOY_LEFT) ||
     mb_buttons.GetState() == BACK_ON){
    ms.back();
    ms.display();
  }
  
  // user selected
  if(joy_stick.IsClicked() || (joy_stick.GetRAState() == JOY_RIGHT)){
    ms.select();
    ms.display();
  }

  // back to main menu
  if(mb_buttons.GetState() == MENU_ON){
    ms.reset();
    ms.display();
  }  
}


/**********************************************************************
NAME

	hand_controller - is a simple controller for AP mounts
		   
SYNOPSIS
	
	hand_controller is a simple controller for AP mounts that
	allows the user to move the AP mount in RA and DEC. There are
	other user selections that help the user who uses PoleMaster.

ARDUINO INTEFACE
	setup()	- set up the handcontroller's serial ports, oled, menu
		  system and GTOCP4 slew rate.

	loop()	- infinite loop to process whatever the user wants to do.

	
AUTHOR
	C.Y. Tan

REVISION
	$Revision: 2ab28b2bf80bc04dcdde59e1b3eb2b4270de41e8 $

SEE ALSO

**********************************************************************/

void setup()
{
  // for debugging
  Serial.begin(115200);
  if(Serial)
    Serial.println(F("Serial port is open"));

  // set up serial port for communicating with AP
  //  Serial1.begin(9600, SERIAL_8N1);
  Serial1.begin(9600);
  if(Serial)
    Serial.println(F("*Serial port 1 is open*"));

  // for the oled  
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Get rid of Sparkfun LOGO in screen memory
  oled.clear(PAGE); // and buffer
  // use 5x7 font
  oled.setFontType(0);

  // flip the display if hardware is version 2.
  #ifdef VERSION2
    oled.flipHorizontal(true);
    oled.flipVertical(true);
  #endif
  
  
  // set up menu sytem
  ms.get_root_menu().add_menu(&mn_nav);
    mn_nav.add_item(&mi_nav);  
    mn_nav.add_item(&mi_rate);
    mn_nav.add_item(&mi_prop_nav);    
  ms.get_root_menu().add_menu(&mn_pm);
    mn_pm.add_item(&mi_sethome);  
    mn_pm.add_item(&mi_ra);
    mn_pm.add_item(&mi_gohome);
  ms.get_root_menu().add_item(&mi_led);
  ms.display();

  // now finally send out the default slew rate
  ap.SendSlewRate(ap.GetSlewRate());
  
}

void loop()
{
  process_menu();
#ifdef AAAAA
  int buf[15];
  if(ap.GetVersion(buf)){
    for(int i=0; i<15; i++){
      Serial.print((char)buf[i]);
      if(buf[i] == '#'){
	Serial.println("");
	break;
      }
    }
  }
#endif
}


