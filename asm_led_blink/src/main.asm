#include "P12LF1571.INC"
;********[Config]*************************************************************************

#define DEBUG_ICD3

#ifdef  DEBUG_ICD3
 __config	_CONFIG1, _WDTE_OFF & _MCLRE_ON & _FOSC_INTOSC & _PWRTE_OFF & _CP_OFF & _BOREN_NSLEEP & _CLKOUTEN_OFF
 __config    _CONFIG2,  _WRT_OFF & _PLLEN_OFF & _STVREN_OFF & _BORV_19 &_LPBOREN_OFF & _LVP_OFF
#else
 __config	_CONFIG1, _WDTE_NSLEEP & _MCLRE_OFF & _FOSC_INTOSC & _PWRTE_ON & _CP_ON & _BOREN_NSLEEP & _CLKOUTEN_OFF
 __config    _CONFIG2,  _WRT_OFF & _PLLEN_OFF & _STVREN_ON & _BORV_19 &_LPBOREN_OFF & _LVP_OFF

#endif

    __idlocs    0x68e8
;*********[Code Start]***************************************************************
	org	00h
  	goto	Start
	goto	Start

	
;*********[Interrupt enter]***************************************************************
	org	04h
        goto    Interrupt


Interrupt:
        goto    Interrupt	


Start:



        banksel PORTA  ;
        clrf    PORTA  ;Init PORTA
        
		BANKSEL   LATA
        clrf      LATA
		
        banksel WPUA
        clrf    WPUA
		
		
        banksel ANSELA  ;bank select analog
        clrf    ANSELA  ;all digital I/O
		
		
		
		
        banksel TRISA
        movlw   b'00111001'
        movwf   TRISA
        
		
		banksel CM1CON0
        clrf    CM1CON0  ; coparator mode not enable
        banksel 0
		
        

LOOP_1
        
		
		movlw   0
        movwf   PORTA
        movlw   b'00000100'
        movwf   PORTA
        
		
        goto    LOOP_1


        END