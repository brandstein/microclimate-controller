#include <Wire.h>  
#include <LiquidCrystal_I2C.h>

// set the LCD address to 0x27 for a 16 chars 2 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address


const int OUTPUT_PIN_INT_TEMP_INC = 8;
const int OUTPUT_PIN_INT_TEMP_DEC = 9;
const int OUTPUT_PIN_HUMIDITY_INC = 10;
const int OUTPUT_PIN_HUMIDITY_DEC = 11;
const int INPUT_PIN_INTERNAL_TEMPERATURE = 0;
const int INPUT_PIN_EXTERNAL_TEMPERATURE = 1;
const int INPUT_PIN_HUMIDITY = 3;
const int INPUT_PIN_PRESSURE = 2;

const int DISABLED = 0;
const int INCREASING = 1;
const int DECREASING = -1;

const char celsius_sign[] = " \337C ";

/* temperature constants; in one-tenths of a degree */
const int HIGH_TEMP = 250;
const int LOW_TEMP = 200;
const int MIN_TEMP = -512;
const int MAX_TEMP = 512;

/* pressure constants; in mmHg */ 
const int MIN_PRESSURE = 650;
const int MAX_PRESSURE = 850;

/* humidity constants; in percents */
const int LOW_HUMIDITY = 40;
const int HIGH_HUMIDITY = 60;


/* data variables */
int value_intTemp;
int value_extTemp;
int value_humidity;
int value_pressure;
int cond_state = DISABLED;
/* state
 *   0 - disabled
 *   1 - increasing 
 *  -1 = decreasing
 *  these constants are defined at constants block
 */
int humidifier_state = 0;

/* delay constants */
const int PAUSE = 3000;

void print_temperature (int val) {
    lcd.setCursor(4, 0);
    lcd.print(value_intTemp / 10.0);
    lcd.print(celsius_sign);
}

void print_humidity(int val) {
    lcd.setCursor(6, 0);
    lcd.print(val / 10.0);
    lcd.print("%");
}

void print_pressure(int val) {
    lcd.setCursor(5, 0);
    lcd.print(val);
    lcd.print(" mmHr");
}

int get_temp_from_voltage(int voltage) {
    int value = voltage * ((MAX_TEMP - MIN_TEMP) / 1024.0);
    value += MIN_TEMP;
    return value;
}

int get_humidity_from_voltage(int voltage) {
    return voltage * 0.9766;
}

int get_pressure_from_voltage(int voltage) {
    return MIN_PRESSURE + voltage * ((MAX_PRESSURE - MIN_PRESSURE) / 1024.0);
}

void setup()   
{
  lcd.begin(16,2);   // initialize the lcd for 16 chars 2 lines, turn on backlight
  pinMode(OUTPUT_PIN_INT_TEMP_INC, OUTPUT);
  pinMode(OUTPUT_PIN_INT_TEMP_DEC, OUTPUT);
  pinMode(OUTPUT_PIN_HUMIDITY_INC, OUTPUT);
  pinMode(OUTPUT_PIN_HUMIDITY_DEC, OUTPUT);
  lcd.clear();
}


void loop() {
    lcd.clear();

    // get data
    value_intTemp = analogRead(INPUT_PIN_INTERNAL_TEMPERATURE);
    value_extTemp = analogRead(INPUT_PIN_EXTERNAL_TEMPERATURE);
    value_humidity = analogRead(INPUT_PIN_HUMIDITY);
    value_pressure = analogRead(INPUT_PIN_PRESSURE);
    
    // process data
    value_intTemp = get_temp_from_voltage(value_intTemp);
    value_extTemp = get_temp_from_voltage(value_extTemp);
    value_humidity = get_humidity_from_voltage(value_humidity);
    value_pressure = get_pressure_from_voltage(value_pressure);

    // print data

/* internal temprature output */
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IN:");
    print_temperature(value_intTemp);
    lcd.setCursor(3, 1);
    if (value_intTemp > HIGH_TEMP) {
        cond_state = DECREASING;
    }
    else if (value_intTemp < LOW_TEMP) {
        cond_state = INCREASING;
    }
    else
        cond_state = DISABLED;
    switch(cond_state) {
        case INCREASING:
            digitalWrite(OUTPUT_PIN_INT_TEMP_DEC, LOW);
            digitalWrite(OUTPUT_PIN_INT_TEMP_INC, HIGH);
            lcd.print("INCREASING");
            break;
        case DECREASING:
            digitalWrite(OUTPUT_PIN_INT_TEMP_INC, LOW);
            digitalWrite(OUTPUT_PIN_INT_TEMP_DEC, HIGH);
            lcd.print("DECREASING");
            break;
        default:
            digitalWrite(OUTPUT_PIN_INT_TEMP_INC, LOW);
            digitalWrite(OUTPUT_PIN_INT_TEMP_DEC, LOW);
            break;
    }
        
    delay(PAUSE);

/* external tempreture output */
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("OUT:");
    print_temperature(value_extTemp);
    delay(PAUSE);

/* humidity output */
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HUM:");
    print_humidity(value_humidity);
    lcd.setCursor(3, 1);
    if (value_humidity / 10.0 > HIGH_HUMIDITY)
        humidifier_state = DECREASING;
    else if (value_humidity / 10.0 < LOW_HUMIDITY)
        humidifier_state = INCREASING;
    else
        humidifier_state = DISABLED;
    switch(humidifier_state) {
        case DECREASING:
            digitalWrite(OUTPUT_PIN_HUMIDITY_INC, LOW);
            digitalWrite(OUTPUT_PIN_HUMIDITY_DEC, HIGH);
            lcd.print("DECREASING");
            break;
        case INCREASING:
            digitalWrite(OUTPUT_PIN_HUMIDITY_DEC, LOW);
            digitalWrite(OUTPUT_PIN_HUMIDITY_INC, HIGH);
            lcd.print("INCREASING");
            break;
        default:
            digitalWrite(OUTPUT_PIN_HUMIDITY_DEC, LOW);
            digitalWrite(OUTPUT_PIN_HUMIDITY_INC, LOW);
            break;
    }
    delay(PAUSE);

/* pressure output */
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PRS:");
    print_pressure(value_pressure);
    delay(PAUSE);
    
}
