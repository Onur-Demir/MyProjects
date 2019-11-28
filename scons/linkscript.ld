/*
(C) 2019 Jesper Ribbe
*/

/* Specify the memory areas */



MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 16K     /* main program */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 16K     /* RAM  */
}



_sram_start = ORIGIN(RAM);
_sram_size = LENGTH(RAM);
_sram_end = _sram_start + _sram_size;
_stack_size = 4k;
_stack_end = _sram_end;     /* default, put stack at top */
_stack_start = _stack_end - _stack_size;



/* Define output sections */
SECTIONS
{
    .bootLoaderConst :
    {
        . = ALIGN(4);
        *(.rodata.bootLoader)
    } >FLASH
    
    .bootLoaderText :
    {
        . = ALIGN(4);
        *(.text.bootLoader)
    } >FLASH
    
    .textHeader :
    {
        . = ALIGN(4);
        *(.rodata.header)
    } >FLASH

    .const :
    {
        . = ALIGN(4);
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text.header)
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        *(.glue_7)         /* glue arm to thumb code */
        *(.glue_7t)        /* glue thumb to arm code */
        *(.eh_frame)

        KEEP (*(.init))
        KEEP (*(.fini))

        . = ALIGN(4);
        _etext = .;        /* define a global symbols at end of code */
    } >FLASH

    .bss :
    {
        . = ALIGN(4);
        *(.bss*)
    } >RAM

    .data : AT(_etext)
    {
        . = ALIGN(4);
        _sdata = .;       /* create a global symbol at data start */
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;       /* define a global symbol at data end */
    } >RAM

}
