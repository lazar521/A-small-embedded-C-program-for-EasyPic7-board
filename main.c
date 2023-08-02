#define CONSTANT 5   //4.882825

// Lcd module connections
sbit LCD_RS at LATB4_bit;
sbit LCD_EN at LATB5_bit;
sbit LCD_D4 at LATB0_bit;
sbit LCD_D5 at LATB1_bit;
sbit LCD_D6 at LATB2_bit;
sbit LCD_D7 at LATB3_bit;
sbit LCD_RS_Direction at TRISB4_bit;
sbit LCD_EN_Direction at TRISB5_bit;
sbit LCD_D4_Direction at TRISB0_bit;
sbit LCD_D5_Direction at TRISB1_bit;
sbit LCD_D6_Direction at TRISB2_bit;
sbit LCD_D7_Direction at TRISB3_bit;
// End Lcd module connections

unsigned long voltageArrRA1[64] = {0};
unsigned long voltageArrRA3[64] = {0};
unsigned arrIndexRA1 = 0;
unsigned arrIndexRA3 = 0;

unsigned long adc_rd,tick;
unsigned long voltageRA1,voltageRA3,newVoltage;

unsigned short adc_completed = 1;
unsigned char idx = 8;
char voltage_str[17] = "Voltage: ";

char convertRA1 = 1;
char displayRA1 = 0;




void interrupt() {

    if(TMR0IF_bit == 1){
       TMR0H = 0xDF; 
       TMR0L = 0xFF;
       ADCON0 |= 0x02; // Start A/D conversion
       TMR0IF_bit = 0;
    }

    if(ADIF_bit == 1){
      adc_rd = ADRESH;
      adc_rd <<= 2;
      adc_rd = (adc_rd & 0x03FC) | (ADRESL >> 6);
      ADIF_bit = 0;                                  // Clear ADIF bit
      adc_completed = 1;

      ADCON0 = ADCON0 & ~(3 << 2);                  // clear bits 2 and 3
      
      if(convertRA1 == 1) ADCON0 |= 0x04;           // decide which pin should be read from next
      else ADCON0 |= 0x0C;
      

      convertRA1 = convertRA1 ^ 0x1;                // toggle convertRA1 flag 
    }


}

void formatInteger(unsigned long nr,char* str){
     str[idx++] = nr/1000 + '0';
     str[idx++] = '.';
     str[idx++] = (nr/100)%10 + '0';
     str[idx++] = (nr/10)%10+'0';
     str[idx++] = 'V';
     str[idx++] = '\0';
     idx = 8;
}


void main() {
  TRISA = 0x0A; // Set RA1 and RA3 pins as input
  ANSELA = 0x0A; // Configure RA1 and R3 pins as analog
  ANSELC = 0x00;
  ADCON0 = 0b00000101;
  UART1_Init(9600);

  ADCON1 = 0b00000000;          // A/D conversion result left justified;
  ADCON2 = 0b00010100;          // A/D acquisition time 4Tad;
                                // A/D conversion clock Fosc/4 = 2MHz
                                
                                
  ADIF_bit = 0; // Clear bits
  GIE_bit = 1;
  PEIE_bit = 1;
  ADIE_bit = 1;
  
  
  ANSELB = 0; // Configure PORTB pins as digital
  Lcd_Init(); // Initialize Lcd
  Lcd_Cmd(_LCD_CLEAR); // Clear display
  Lcd_Cmd(_LCD_CURSOR_OFF); // Cursor off

  voltageRA1 = 0;               //initialize sums to 0
  voltageRA3 = 0;
  
  tick = 0;
  T0CON = 0x88; // Set TMR0 in 16bit mode, no prescaler
  TMR0IE_bit = 1; // Enable TMR0 overflow interrupt
  TMR0H = 0xC1; 
  TMR0L = 0x7F;
  
  
  while (1) {
    if (adc_completed) {

        if(displayRA1 == 1){
          newVoltage = (adc_rd * CONSTANT)>>6;
          voltageRA1 = voltageRA1 - voltageArrRA1[arrIndexRA1] + newVoltage;
          voltageArrRA1[arrIndexRA1] = newVoltage;
          arrIndexRA1++;
          if(arrIndexRA1 == 64){ arrIndexRA1 = 0; }

          formatInteger(voltageRA1,voltage_str);
          Lcd_Out(1, 1, voltage_str);
          
          displayRA1 = 0;
        }
        if(displayRA1 == 0){
        newVoltage = (adc_rd * CONSTANT)>>6;
        voltageRA3 = voltageRA3 - voltageArrRA3[arrIndexRA3] + newVoltage;
        voltageArrRA3[arrIndexRA3] = newVoltage;
        arrIndexRA3++;
         if(arrIndexRA3 == 64){ arrIndexRA3 = 0; }
        
        formatInteger(voltageRA3,voltage_str);
        Lcd_Out(2, 1, voltage_str);

        displayRA1 = 1;
      }
      adc_completed = 0;
    }
  }
}
