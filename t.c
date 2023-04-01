#include <htc.h>

__CONFIG(FOSC_HS & WDTE_OFF & PWRTE_OFF & MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF & IESO_OFF & FCMEN_OFF & LVP_OFF);
__CONFIG(BOR4V_BOR40V & WRT_OFF);

#define _XTAL_FREQ 4000000

// Ngan, tram, chuc, don vi
unsigned char ng_dig, tr_dig, ch_dig, dv_dig;

void disp7segmul4() {
    unsigned char scancode, i;
    unsigned char a[4];
    scancode = 0x01;
    a[0] = dv_dig;
    a[1] = ch_dig;
    a[2] = tr_dig;
    a[3] = ng_dig;
    for (i = 0; i <= 3; i++) {
        PORTD = a[i];
        PORTC = ~scancode;
        __delay_ms(3);
        PORTC = 0xFF;
        scancode = scancode << 1;
    }
}



// ***************************************************
// Ham giai ma tu so BCD nen sang ma 7 doan (2 so BCD nen).
// ***************************************************
// LED: Anode chung, kich hoat muc 0, 7 doan: DP (MSB) -> A (LSB)
// Gia tri giai ma: 0 -> 9.
// ***************************************************
// Ten: bcd4to7seg(bcd2_dig,bcd1_dig,ng_dig,tr_dig,ch_dig,dv_dig).
// Dau vao: bcd2_dig = So BCD nen (NGAN - TRAM).
// bcd1_dig = So BCD nen (CHUC - DON VI).
// Dau ra: ng_dig,tr_dig = Gia tri 7 doan (NGAN - TRAM).
// ch_dig,dv_dig = Gia tri 7 doan (CHUC - DON VI).
void bcd4to7seg(unsigned char bcd2_dig, unsigned char bcd1_dig) {
    // Khai bao bien.
    unsigned char i;
    // Khai bao hang.
    const unsigned char a[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
    // Dinh nghia ham.
    i = bcd1_dig & 0x0F;
    dv_dig = a[i];
    i = bcd1_dig & 0xF0;
    i = i >> 4;
    ch_dig = a[i];
    i = bcd2_dig & 0x0F;
    tr_dig = a[i];
    i = bcd2_dig & 0xF0; // Tach lay so BCD hang ngan.
    i = i >> 4;
    ng_dig = a[i]; // Lay ma 7 doan tuong ung cho BCD hang ngan.
}


// ***************************************************
// Ham tao thoi gian tre co quet LED 7 doan de hien thi.
// ***************************************************
// Ten: delayscan(time_del).
// Dau vao: time_del: Thoi gian tre (ms).
// ng_dig,tr_dig = Gia tri hien thi (NGAN - TRAM).
// ch_dig,dv_dig = Gia tri hien thi (CHUC - DON VI).
// Dau ra:
// ***************************************************
void delayscan(unsigned int time_del) {
    // Khai bao bien.
    unsigned char i;
    // Dinh nghia ham.
    for (i = 1; i <= (time_del / 12); i++) {
        disp7segmul4();
    }
}


// ***************************************************
// Ham chuyen doi so hex 8 bit (<=99) sang so BCD nen.
// ***************************************************
// Ten: hex8tobcd99.
// Dau vao: x = Gia tri HEX (00 - 99).
// Dau ra: y = Gia tri BCD nen (00 - 99).
// ****************************************************/
unsigned char hex8tobcd99(unsigned char x) {
    // Khai bao bien.
    unsigned char y;
    // Dinh nghia ham.
    y = (x / 10) << 4;
    y = y | (x % 10);
    return (y);
}
// Chuong trinh chinh.

void main(void) {
    unsigned int dat;
    unsigned char bcd2_dig, bcd1_dig, dat2_disp, dat1_disp;
    
    // Khai bao port
    ANSEL = ANSELH = 0;
    PORTD = 0X00;
    TRISD = 0X00;
    PORTC = 0X00;
    TRISC = 0X00;
    
    while (1) {
        // Bien dem bat dau tu 1300 giam dan xuong 1234
        dat = 1300;
        while(dat >= 1234) {
            // Hang ngan-tram
            dat2_disp = dat / 100;
            // Hang chuc-don vi
            dat1_disp = dat % 100;
            // Chuyen sang dang bcd nen
            bcd2_dig = hex8tobcd99(dat2_disp);
            bcd1_dig = hex8tobcd99(dat1_disp);
            // hien thi ra led 7
            bcd4to7seg(bcd2_dig, bcd1_dig);
            delayscan(300);
            --dat;
        }
    }
}