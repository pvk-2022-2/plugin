ENTRY (__start)

EXEC_OFFSET = 0x1000;
STACK_SIZE = 0x4000;

SECTIONS
{
    . = EXEC_OFFSET;

    .text :
    {
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }

    .bss :
    {
        *(COMMON)
        *(.bss*)

        . = ALIGN (16);
            s_stack_bottom = .;
            . += STACK_SIZE;
            s_stack_top = .;
    }    

    /DISCARD/ :
    {
        *(.interp)
    }
}