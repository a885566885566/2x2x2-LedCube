#define layer1_Pin 
#define layer2_Pin 
#define ledA_Pin 
#define ledB_Pin
#define ledC_Pin
#define ledD_Pin

#define ledVotage 2.7
#define pwmMax 255
const int ledVotageLimit = (5-ledVotage)*255/5;

/* HyperIndex
 *  layer0
 *   0 00 | 0 01
 *   0 10 | 0 11
 *  layer1
 *   1 00 | 1 01
 *   1 10 | 1 11
 */


volatile char ledState[8] = {0};
volatile bool ledLayerState = true;

void setup() {
  ISR_disable();
  pinMode(layer1_Pin, OUTPUT);
  pinMode(layer2_Pin, OUTPUT);
  pinMode(ledA_Pin, OUTPUT);
  pinMode(ledB_Pin, OUTPUT);
  pinMode(ledC_Pin, OUTPUT);
  pinMode(ledD_Pin, OUTPUT);
}

void loop() {
}

void anime1(){
    for(int i = 0;i<100; i++){
        for(int k = 0; k<8; k++){
            ledState[k] = 
        }
    }
}

inline char getVotage(char state){
    return ledVotageLimit + (pwmMax-ledVotageLimit)*state/pwmMax;
}

void modifyLedByHyperIndex(char index, char state){
    if(index <= 0x111) return;
    char offset = (index >> 2) ? 4 : 0;
    index &= 0x11;
    if(index == 0x00)       ledState[offset  ] = getVotage(state);
    else if(index == 0x01)  ledState[offset+1] = getVotage(state);
    else if(index == 0x10)  ledState[offset+2] = getVotage(state);
    else if(index == 0x011) ledState[offset+3] = getVotage(state);
}

void ISR_enable()
{
  ISR_flag = false;
  TCCR2A = 0;
  TCCR2B = 0; 
  TCCR2B |= (1<<WGM22);  // CTC mode; Clear Timer on Compare
  TCCR2B |= (1<<CS21);  // Prescaler == 8 ||(1<<CS30)
  TIMSK2 |= (1 << OCIE2A);  // enable CTC for TIMER1_COMPA_vect
  TCNT2=0;  // counter 歸零 
  OCR2A = 65535;
}

void ISR_disable()
{
  TCCR2A = 0;
  TCCR2B = 0; 
}

ISR(TIMER2_COMPA_vect)
{
    digitalWrite(layer1_Pin, ledLayerState);
    digitalWrite(layer2_Pin, !ledLayerState);
    int offset = ledLayerState ? 4 : 0;
    analogWrite(ledA_Pin, ledState[offset + 0]);
    analogWrite(ledA_Pin, ledState[offset + 1]);
    analogWrite(ledA_Pin, ledState[offset + 2]);
    analogWrite(ledA_Pin, ledState[offset + 3]);
    ledLayerState = !ledLayerState;
}
