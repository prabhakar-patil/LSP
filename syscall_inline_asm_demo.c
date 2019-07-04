
/*
   $ gcc -o main main.c
   $ ./main
   $ echo $?   -> you will see  10 (find where it is written in code)
*/

char msg[] = "Hello, kernel trap\n";
int main(void)
{
        int ret;

        //write(stdout, msg, strlen(msg));
        /*
                movl $4, %eax    #__NR_write
                movl $1, %ebx    # stdout
                movl $msg, %ecx  # msg
                movl $19, %edx   # strlen(msg)
                int  $0x80       # trap instruction
        */
        __asm__("\n\tmovl $4, %eax\n\tmovl $1, %ebx\n\tmovl $msg, %ecx\n\tmovl $19, %edx\n\tint $0x80\n");



        //exit(0)
        /*
                movl $1, %eax    #__NR_exit
                movl $0, %ebx    # 0
                int  $0x80       # trap instruction
        */
        __asm__("\n\tmovl $1, %eax\n\tmovl $10, %ebx\n\tint $0x80\n");
}
