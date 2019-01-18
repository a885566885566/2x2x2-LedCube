/*
 * National ChengKung University
 * 2019 Engineering Science Campus
 * 2x2x2 Led Cube
 */

#define layer1_Pin A0
#define layer2_Pin A1
#define ledA_Pin 3
#define ledB_Pin 5
#define ledC_Pin 10
#define ledD_Pin 9

#define ledVotage 2.7
#define pwmMax 255
const int ledVotageLimit = (5-ledVotage)*255/5;
const char ledMap[2][2][2] = {
    {{0x100, 0x101},{0x110, 0x111}},
    {{0x000, 0x001},{0x010, 0x011}}
}
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
  pinMode(A3, INPUT);
  randomSeed(analogRead(A3));
  pinMode(layer1_Pin, OUTPUT);
  pinMode(layer2_Pin, OUTPUT);
  pinMode(ledA_Pin, OUTPUT);
  pinMode(ledB_Pin, OUTPUT);
  pinMode(ledC_Pin, OUTPUT);
  pinMode(ledD_Pin, OUTPUT);
  ISR_enable();
}

void loop() {
    
    anime1();
}

/*
 * spark
 */
void anime_spark(int times=10, int fre=800, bool speedUp = true){
    const int minimumFre = 10;
    for(int i = 0;i<times; i++){
        for(int k = 0; k<8; k++)
            ledState[k] = getVotage( 255 );
        delay(fre);
        for(int k = 0; k<8; k++){
            ledState[k] = getVotage( 0 );
        }
        delay(fre);
        if(speedUp){
            fre = minimumFre + (times-i)*(fre-minimumFre)/times;
        }
    }
}
//getVotage( (sin(millis()/period)+1)*120 );
/*
 * Point moving
 */
void anime_pointMoving(int times=100, int fre=300){
    static char index = 0x101;
    for(int i = 0; i<times; i++){
        modifyLedByHyperIndex(index, 0);
        randomChoose(&index);
        modifyLedByHyperIndex(index, 255);
        delay(fre);
    }
}

/*
 * Triangle moving
 */
void anime_triangleMoving(int times=100, int fre=300){
    static char head = 0x000;
    static char tail = 0x001;
    for(int i = 0; i < times; i++){
        modifyLedByHyperIndex(tail, 0);
        tail = head;
        randomChoose(&head);
        modifyLedByHyperIndex(head, 255);
        delay(fre);
    }
}

void anime_circleRuning(int times=100, int fre=300){
    static char upper = 0;
    static char lower = 0;
    static bool layer = false;
    const char arr[4] = {0x00, 0x01, 0x11, 0x10};
    for(int i=0;i<times;i++){
        modifyLedByHyperIndex((layer ? 0 : 0x100) | arr[upper], 255);
        modifyLedByHyperIndex((layer ? 0x100 : 0) | arr[lower], 255);
        upper = (upper>=4 ? 0 : upper+1);
        lower = (lower<=0 ? 3 : lower-1);
        delay(fre);
        if(i%(300/10)) layer = !layer;
    }
}

void anime_lightUpSmooth(){
    static 
}
void anime_planeFlip(){
    static char plane[4] = {0x000, 0x001, 0x010, 0x011};
    static char edge[2] = {0, 0};
    edge[0] = (char)random(0, 3);
    do{
        edge[1] = (char)random(0, 3);
    }while( !((planeIndex == prePlaneIndex) && (planeScale == prePlaneScale)) );
    prePlaneIndex = planeIndex;
    prePlaneScale = planeScale;

    for(int x=0; x<2; x++)
        for(int y=0; y<2; y++)
            for(int z=0; z<2; z++){
                if(planeIndex == 0x100 && ) 
            }
}
void randomChoose(char *index){
    /*
     * index   ->  0x111
     * select  ->  0x010
     * mask    ->  0x101
     * output  ->  0x101
     */
    char rnd = (char)random(0, 2);
    char select = 1 << rnd; // Offset to target bit
    char mask = !select;    // Filter mask bits
    select &= *index;       // Get the origin value of target bit
    *index &= mask;         // Get data except for the target bit
    *index |= !select;      // Combine the two data
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
  TCCR2A = 0;
  TCCR2B = 0; 
  TCCR2B |= (1<<WGM22);  // CTC mode; Clear Timer on Compare
  TCCR2B |= (1<<CS21);  // Prescaler == 8 ||(1<<CS30)
  TIMSK2 |= (1 << OCIE2A);  // enable CTC for TIMER1_COMPA_vect
  TCNT2=0;  // counter 歸零 
  OCR2A = 10000;
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
    analogWrite(ledA_Pin, ledState[offset    ]);
    analogWrite(ledB_Pin, ledState[offset + 1]);
    analogWrite(ledC_Pin, ledState[offset + 2]);
    analogWrite(ledD_Pin, ledState[offset + 3]);
    ledLayerState = !ledLayerState;
}
