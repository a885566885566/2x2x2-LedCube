#define length_x 3      // Control_line_x 的數量
#define length_y 4      // Control_line_y 的數量
#define length_map 3    // 畫格數量

// 設定控制線(Pin腳編號)
const int control_line_x[length_x] = {2, 3, 4, 5};
const int control_line_y[length_y] = {6, 7, 8};

// 畫格編號初始化
volatile int map_idx = 0;

// 畫格矩陣
const bool ledMap[][length_x][length_y]={
    {{1, 1, 1, 1},
    {1, 0, 0, 1},
    {1, 1, 1, 1}},

    {{1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1}},

    {{1, 1, 1, 1},
    {0, 0, 0, 0},
    {1, 1, 1, 1}}
    };

void setup(){
    // 將control_line_x與control_line_y都寫為低電壓，使LED Cube進入LED矩陣模式
    for(int i =0; i<length_x; i++){
        pinMode(control_line_x[i], OUTPUT);
        digitalWrite(control_line_x[i], LOW);
    }
    for(int i =0; i<length_y; i++){
        pinMode(control_line_y[i], OUTPUT);
        digitalWrite(control_line_y[i], LOW);
    }

    delay(3000);    // 等待LED Cube進入LED矩陣模式
    ISR_enable();   // 啟動中斷(畫格自動根據map_idx更新)
}

void loop(){
    map_idx = (map_idx+1) % length_map; // 使畫格切換(map_idx不斷遞增)
    delay(1000);                        // 畫格間隔
}

/* 
 * 利用Timer作畫格更新，自動根據map_idx的編號選擇畫格，並輸出到control_line
 */
// 中斷計時器設定
void ISR_enable()
{
  TCCR2A = 0;
  TCCR2B = 0; 
  TCCR2B |= (1<<WGM22);             // CTC mode; Clear Timer on Compare
  TCCR2B |= (1<<CS22) | (1<<CS20);  // Prescaler == 8 ||(1<<CS30)
  TIMSK2 |= (1 << OCIE2A);          // enable CTC for TIMER1_COMPA_vect
  TCNT2=0;                          // counter 歸零 
  OCR2A = 1000;
}

void ISR_disable()
{
  TCCR2A = 0;
  TCCR2B = 0; 
}

// 中斷副程式
ISR(TIMER2_COMPA_vect)
{
    static volatile int update_idx = 0;
    digitalWrite(control_line_x[update_idx], false);    // 將control_line_x寫為低電壓(關閉上一次x的輸入)
    update_idx = (update_idx+1) % length_x;             // 前往下一個x
    for (int i=0; i<length_y; i++){                     // 根據畫格矩陣更新control_line_y
        digitalWrite(control_line_y[i], ledMap[map_idx][update_idx][i]);
    }
    digitalWrite(control_line_x[update_idx], true);     // 將control_line_x寫為高電壓
}
