/*************************************************************
 * @file        : Project_Adi.ino
 * @brief       : NodeMcu
 * @revision    : 0.0.0
 * @date        : 15 August 2021
 * @environment : Arduino 1.8.13
 * @supported by: Samikro.id
 * **********************************************************/

/************************ Includes **************************/
#include <PZEM004Tv30.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

/************************ Defines  **************************/
#define LED

#define BLYNK_PRINT     Serial

#define SENSOR_UPDATE   1000

#ifdef LED
#define LED_PIN         LED_BUILTIN
#define LED_BLYNK_TIME  200
#endif

/*  PIN MAP     */
#define PZEM1_RX_PIN    12          // D6   NodeMcu
#define PZEM1_TX_PIN    13          // D7   NodeMcu

#define PZEM2_RX_PIN    16          // D0   NodeMcu
#define PZEM2_TX_PIN    14          // D5   NodeMcu

/*  REDEFINED   */
#define SERIAL1_RX_PIN  PZEM1_TX_PIN
#define SERIAL1_TX_PIN  PZEM1_RX_PIN

#define SERIAL2_RX_PIN  PZEM2_TX_PIN
#define SERIAL2_TX_PIN  PZEM2_RX_PIN

/************************ Macros ****************************/
PZEM004Tv30 pzem1(SERIAL1_RX_PIN, SERIAL1_TX_PIN);
PZEM004Tv30 pzem2(SERIAL2_RX_PIN, SERIAL2_TX_PIN);

/************************ Structs ***************************/
typedef struct{
    float voltage;
    float current;
    float power;
    float energy;
    float frequency;
    float pf;
}PzemData_Struct;

/*********************** Variables **************************/
/*  STATIC VARIABLES    */
static char auth[] = "fill auth here";
static char ssid[] = "fill ssid wifi target here";
static char pass[] = "fill password wifi target here";

/*  GLOBAL VARIABLES    */
PzemData_Struct pzem1Data;
PzemData_Struct pzem2Data;

uint32_t sensor_time;

#ifdef LED
uint32_t led_time;
bool led_on;
#endif

void setup() {
    Serial.begin (9600);

    Blynk.begin (auth, ssid, pass, "blynk-cloud.com", 8080);

    sensor_time = millis() + SENSOR_UPDATE;

#ifdef LED
    led_time = millis();
    led_on = true;
#endif
}

void loop() {
    ledBlynk();

    if((millis() - sensor_time) >= SENSOR_UPDATE){
        sensor_time += SENSOR_UPDATE;

        readPzem1();
        Serial.println();
        Blynk.virtualWrite(V0, pzem1Data.power);
        Blynk.virtualWrite (V1, pzem1Data.energy);
        Blynk.virtualWrite(V2, pzem1Data.voltage);
        Blynk.virtualWrite (V3, pzem1Data.current);

        readPzem2();
        Serial.println();
        Blynk.virtualWrite(V4, pzem2Data.power);
        Blynk.virtualWrite (V5, pzem2Data.energy);
        Blynk.virtualWrite(V6, pzem2Data.voltage);
        Blynk.virtualWrite (V7, pzem2Data.current);
    }
    
    Blynk.run();
}

/*  Baca Pzem1  */
bool readPzem1(){
    PzemData_Struct pzem;
    bool valid = false;

    pzem.voltage    = pzem1.voltage();      // baca tegangan
    pzem.current    = pzem1.current();      // baca arus
    pzem.power      = pzem1.power();        // baca power/daya
    pzem.energy     = pzem1.energy();       // baca energy
    pzem.frequency  = pzem1.frequency();    // baca frequency
    pzem.pf         = pzem1.pf();           // baca power factor (pf)

    if(isnan(pzem.voltage) || isnan(pzem.current) || isnan(pzem.power) 
        || isnan(pzem.energy) || isnan(pzem.frequency) || isnan(pzem.pf)){
        pzem1Data.voltage    = 0;
        pzem1Data.current    = 0;
        pzem1Data.power      = 0;
        pzem1Data.energy     = 0;
        pzem1Data.frequency  = 0;
        pzem1Data.pf         = 0;
    }           
    else{
        pzem1Data.voltage    = pzem.voltage;
        pzem1Data.current    = pzem.current;
        pzem1Data.power      = pzem.power;
        pzem1Data.energy     = pzem.energy;
        pzem1Data.frequency  = pzem.frequency;
        pzem1Data.pf         = pzem.pf;

        valid = true;
    }

    if(valid){
        Serial.println("------------ PZEM1 -----------");
        Serial.print(pzem1Data.voltage, 2);         Serial.println("V");
        Serial.print(pzem1Data.current, 3);         Serial.println("A");
        Serial.print(pzem1Data.power, 2);           Serial.println("W");
        Serial.print(pzem1Data.energy, 2);          Serial.println("Wh");
        Serial.print(pzem1Data.frequency, 2);       Serial.println("Hz");
        Serial.print("PF : ");                      Serial.println(pzem1Data.pf, 3);
    }else{
        Serial.println("-- READ PZEM1 FAILED --");
    }
    Serial.println();

    return valid;
}

/*  Baca Pzem2  */
bool readPzem2(){
    PzemData_Struct pzem;
    bool valid = false;

    pzem.voltage    = pzem2.voltage();      // baca tegangan
    pzem.current    = pzem2.current();      // baca arus
    pzem.power      = pzem2.power();        // baca power/daya
    pzem.energy     = pzem2.energy();       // baca energy
    pzem.frequency  = pzem2.frequency();    // baca frequency
    pzem.pf         = pzem2.pf();           // baca power factor (pf)

    if(isnan(pzem.voltage) || isnan(pzem.current)|| isnan(pzem.power) 
        || isnan(pzem.energy) || isnan(pzem.frequency) || isnan(pzem.pf)){
        pzem2Data.voltage    = 0;
        pzem2Data.current    = 0;
        pzem2Data.power      = 0;
        pzem2Data.energy     = 0;
        pzem2Data.frequency  = 0;
        pzem2Data.pf         = 0;
    }
    else{
        pzem2Data.voltage    = pzem.voltage;
        pzem2Data.current    = pzem.current;
        pzem2Data.power      = pzem.power;
        pzem2Data.energy     = pzem.energy;
        pzem2Data.frequency  = pzem.frequency;
        pzem2Data.pf         = pzem.pf;

        valid = true;
    }

    if(valid){
        Serial.println("------------ PZEM2 -----------");
        Serial.print(pzem2Data.voltage, 2);         Serial.println("V");
        Serial.print(pzem2Data.current, 3);         Serial.println("A");
        Serial.print(pzem2Data.power, 2);           Serial.println("W");
        Serial.print(pzem2Data.energy, 2);          Serial.println("Wh");
        Serial.print(pzem2Data.frequency, 2);       Serial.println("Hz");
        Serial.print("PF : ");                      Serial.println(pzem2Data.pf, 3);
    }else{
        Serial.println("-- READ PZEM2 FAILED --");
    }
    Serial.println();

    return valid;
}

/**
 * indikator led berkedip jika sudah berjalan di loop 
 * jika led tidak berkedip, ada program yang hang
 * */
void ledBlynk(){
#ifdef LED
    if((millis() - led_time) >= LED_BLYNK_TIME){
        led_time += LED_BLYNK_TIME;

        if(led_on == true){ 
            digitalWrite(LED_PIN, LOW); 
            led_on = false;
        }
        else            {   
            digitalWrite(LED_PIN, HIGH);
            led_on = true;
        }
    }
#endif
}