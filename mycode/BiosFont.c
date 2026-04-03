/*****************************************************
* FILE: BiosFont.c
* 폰트 변환 함수
*******************************************************/
/* Include */
	/* */
	#include "main.h"
	#include "BiosFont.h"

/* Define */
word KStoKSSM(byte a, byte b)
{
	byte first,second;
	byte r1,r2;

	word index;

	if((a>0)&&(b>0)){            
		if(a&0x80){
			first=a;second=b;
			if(first>=0xb0&&first<=0xc8){     //완성형 한글인가 확인
				if (second < 0xa1 || second > 0xfe){
					r1=first;r2=second;
				} else {
					index=(first-0xb0)*94+second-0xa1; // 0 ~ 2350
					r1 = _HanCodeTable[index*2];  
					r2 = _HanCodeTable[index*2+1];
				}

			}else{
				r1 = first;
				r2 = second;
			}
		}else{ 
			r1=a;r2=b;
		}
		index=((r1<<8)|r2);
	}
	return index;
}

void GetEngFont(byte code, byte *buffer)
{
	word offset, loop;
	
	//if(code<32) return;         // space문자 이전의 것은 무시됨 
	//offset=(code-32)*16;
	offset = code * 16;
	for(loop=0;loop<16;loop++) buffer[loop] = ASCII8x16_MY[loop+offset];
}


void GetKlFont(word code, byte *buffer, byte mag)
{
	word first,middle,last,loop,offset;
	byte temp[32], buf;
	//byte i, j=0, Val1=0, Val2=0;

	first=_first[(code>>10)&31];
	middle=_middle[(code>>5)&31];
	last=_last[(code)&31];
	if(last==0) {
		offset=cho[middle]*640;                  //초성
		offset+=first*32;
		for(loop=0;loop<32;loop++) temp[loop] = HAN16X16[loop+offset];

		if(first==1||first==24) offset=5120;	// 중성
		else offset=5120+704;

		offset+=middle*32;
		for(loop=0;loop<32;loop++) temp[loop] |= HAN16X16[loop+offset]; // HF3 = 벌수 H3 = 글자번호

	} else {
		offset=cho2[middle]*640;                   //초성
		offset+=first*32;
		for(loop=0;loop<32;loop++) temp[loop] = HAN16X16[loop+offset];

		if(first==1||first==24) offset=5120+704*2; //중성
		else offset=5120+704*3;

		offset+=middle*32;
		for(loop=0;loop<32;loop++) temp[loop] |= HAN16X16[loop+offset]; // HF3 = 벌수 H3 = 글자번호

		offset=5120+2816+jong[middle]*896;         //종성
		offset+=last*32;
		for(loop=0;loop<32;loop++) temp[loop] |= HAN16X16[loop+offset]; // HF3 = 벌수 H3 = 글자번호
	}
	
	if(mag) {
		for(loop=0;loop<16;loop++) {
			buf = temp[loop*2];
			buffer[loop*2*4]   = abMag_2X[buf>>4];
			buffer[loop*2*4+1] = abMag_2X[buf&0x0F];
			buffer[loop*2*4+4] = buffer[loop*2*4];
			buffer[loop*2*4+5] = buffer[loop*2*4+1];		
		}
	
		for(loop=0; loop<16; loop++) {
			buf = temp[loop*2+1];
			buffer[loop*2*4+2] = abMag_2X[buf>>4];
			buffer[loop*2*4+3] = abMag_2X[buf&0x0F];
			buffer[loop*2*4+6] = buffer[loop*2*4+2];
			buffer[loop*2*4+7] = buffer[loop*2*4+3];
		}
	} else {
		for(loop=0;loop<32;loop++) {
			buffer[loop] = temp[loop];				
		}
	}
}

extern const byte ASCII8x16_GO[];
extern const byte ASCII8x16_MY[];
void GetEnFont(byte code, byte *buffer, byte mag)
{
	byte i;
	byte temp[16], buf;

	for(i=0;i<16;i++) {
		temp[i] = ASCII8x16_GO[code * 16 + i];//8*16
		//temp[i] = ASCII8x16_MY[code * 16 + i];//8*16
	}
	
	if(mag) {
		for(i=0;i<16;i++) {
			buf = temp[i];
			buffer[i*4]   = abMag_2X[buf>>4];
			buffer[i*4+1] = abMag_2X[buf&0x0F];
			buffer[i*4+2] = buffer[i*4];
			buffer[i*4+3] = buffer[i*4+1];
		}
	} else {
		for(i=0;i<16;i++) {
			buffer[i] = temp[i];//8*16
		}
	}
}