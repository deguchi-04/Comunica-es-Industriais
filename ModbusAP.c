#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define MIN_REG 1
#define MAX_REG 123
#define FUNC_WMR 0x10
#define FUNC_RHR 0x03
#define APDU_LEN 6
#define APDU_R_LEN 2*n_r + 6

int send_modbus_request (char *server_add, int port, uint8_t *APDU, int APDUlen, uint8_t *APDU_R);
//APDU sequencia de bytes que modbus AL envia p modbus tcp + MBAP
//APDUlen saber quantos bytes tenho em APDu
//APDU_R resposta do APDU 



int write_multiple_regs (char *server_add, int port, uint16_t st_r, uint16_t n_r, uint16_t *val) {
//st_r adress destino
//n_r numero de bytes  

    uint8_t *APDU = (uint8_t *) calloc(APDU_LEN, sizeof(uint8_t));//inicializa um vetor a 0 com o tamanho necessario
    uint8_t *APDU_R = (uint8_t *) calloc(APDU_R_LEN, sizeof(uint8_t));
    int b = 2*n_r;//ver no livro MODBUS, 
    int cnt = 0;//inicializar um contador

    //Ver se os parametros são validos
    if (n_r<MIN_REG || n_r>MAX_REG) 
    {
        return -1;
    }
    //assembles APDU (MODBUS PDU)
    APDU[0] = FUNC_WMR;
    APDU[1] = st_r/256; //Ah
    APDU[2] = st_r%256; //Al
    APDU[3] = n_r/256; //Ch count high
    APDU[4] = n_r%256; //CL count low
    APDU[5] = b; //B
    for (int i = 0; i < b; i++) {
        APDU[i+APDU_LEN]=(uint8_t)(val[cnt]/256);
            i++;send_modbus_request(server_add,port,APDU,5,APDU_R);
        APDU[i+APDU_LEN]=(uint8_t)(val[cnt]%256);
            cnt++;
    }
    send_modbus_request(server_add,port,APDU, b+6,APDU_R);
    
    //check the response (APDU_R or error code)
    if (APDU_R[0] == 0X90) {
        if (APDU_R[1] == 0X01) {
            printf("wrong function code\n");
            
            return 1;
        }
        else if (APDU_R[1] == 0X02) {
            printf("starting adress + byte count not right\n");
           
            return 2;
        }
        else if (APDU_R[1] == 0X03) {
            printf("registers or byte count not right\n");
            
            return 3;
        }
        else if (APDU_R[1] == 0X04) {
            printf("Function write not right\n");
           
            return 4;
        }
    }
    //return number of written registers or error
    else if (APDU_R[0] == FUNC_WMR) {
        //return (256*APDU_R[3]+APDU_R[4]);
       
        return 0;
    }

    else 
    {;
        return -1;
    }
}


int read_h_regs (char *server_add, int port, uint16_t st_r, uint16_t n_r, int16_t *val) {

    uint8_t *APDU = (uint8_t *) calloc(APDU_LEN, sizeof(uint8_t));
    uint8_t *APDU_R = (uint8_t *) calloc(APDU_R_LEN, sizeof(uint8_t));
    int address, cnt;

    //check consistency of parameters
    if (n_r<MIN_REG || n_r>MAX_REG+2) {
        
        return -1;
    }

    //assembles APDU (MODBUS PDU)
    APDU[0] = FUNC_RHR;
    APDU[1] = st_r/256; //Ah
    APDU[2] = st_r%256; //Al
    APDU[3] = n_r/256; //Ch count high
    APDU[4] = n_r%256; //CL count low

    send_modbus_request(server_add,port,APDU,5,APDU_R);

     //check the response (APDU_R or error code)
    if (APDU_R[0] == 0X83) {
        if (APDU_R[1] == 0X01) {
            printf("wrong function code\n");
            
            return 1;
        }
        else if (APDU_R[1] == 0X02) {
            printf("starting adress + byte count not right\n");
            return 2;
        }
        else if (APDU_R[1] == 0X03) {
            printf("registers or byte count not right\n");
           
            return 3;
        }
        
        else if (APDU_R[1] == 0X04) {
            printf("Function read not right\n");
            return 4;
        }
    }
    //return number of read registers or error
    else if (APDU_R[0] == FUNC_RHR) {
        for (int i = 0; i < n_r; i++) {
            address = st_r + i;
            val[i] = (APDU_R[2+2*i]*256)+APDU_R[3+2*i];
            printf("Valor do registo %d: %d\n", address, val[i]);

        }
        //return (APDU_R[1]);
        return 0;
    }
    else{
        
        return -1;
    } 
}