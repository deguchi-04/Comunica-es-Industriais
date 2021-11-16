#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define MB_PORT 5502
#define APDU_LEN 6
#define SERVER_ADD "127.0.0.1"
#define MAX_REG 123
#define MIN_REG 1
#define MAX_ADDR 65535
#define MIN_ADDR 0


int write_multiple_regs (char *server_add, int port, uint16_t st_r, uint16_t n_r, uint16_t *val);
int read_h_regs (char *server_add, int port, uint16_t st_r, uint16_t n_r, int16_t *val);

int main()
{
    
    static uint16_t reg_add, n_reg, add;

    printf("how many registers want to write? ");
    scanf("%hd", &n_reg);
    if (n_reg < MIN_REG || n_reg > MAX_REG) {
        printf("REGISTERS ERROR\n");
        
        return -1;
    }
    
    uint16_t *reg = (uint16_t *) calloc(n_reg, sizeof(uint16_t));
    printf("First reg address: ");
    scanf("%hd", &reg_add);
    if (reg_add < MIN_ADDR || reg_add > MAX_ADDR) {
        printf("ADDRESS ERROR\n");
        
        return -1;
    }

    for (int i = 0; i < n_reg; i++) {
        add = reg_add+i;
        printf("Value tou wanna send to %d: ", add);
        scanf("%hd", &reg[i]);
    }
    
    
    int len = write_multiple_regs(SERVER_ADD, MB_PORT, reg_add, n_reg, reg);
    printf("Retorno %d \n", len);
    if (len == -1) {
        printf("ERROR\n");
        
        return -1;
    }
    printf("Num of registers: %d\n\n", n_reg);


    printf("Read register\nhow many registers want to read? ");
    scanf("%hd", &n_reg);
    if (n_reg< MIN_ADDR || n_reg> MAX_REG) {
        printf("INVALID REGISTERS\n");

        return -1;
    }

    printf("First reg address: ");
    scanf("%hd", &reg_add);
    if (reg_add< MIN_ADDR || reg_add> MAX_ADDR) {
        printf("INVALID ADDRESS\n");
        
        return -1;
    }

    
    len = read_h_regs(SERVER_ADD, MB_PORT, reg_add, n_reg, reg);
    printf("Retorno %d \n", len);
    if (len == -1) {
        printf("ERRO\n");
        
        return -1;
    }
   
    return 0;
}
