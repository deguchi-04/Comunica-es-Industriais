#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define PDU_LEN 6
#define UID 1

int send_modbus_request (char* server_add, int port, uint8_t* APDU, int APDUlen, uint8_t* APDU_R) {

    //generate trasaction identifier
    static int next_ti = 0;
    next_ti++;

    //assemple PDU = APDU + MBAP

    uint8_t *PDU = (uint8_t *) calloc(PDU_LEN, sizeof(uint8_t));
    uint8_t *PDU_R = (uint8_t *) calloc(PDU_LEN, sizeof(uint8_t));

    PDU[0] = (uint8_t)next_ti/256;
    PDU[1] = (uint8_t)next_ti%256;
    PDU[2] = 0;
    PDU[3] = 0;
    PDU[4] = (uint8_t)(APDUlen + 1)/256;
    PDU[5] = (uint8_t)(APDUlen + 1)%256;
    PDU[6] = UID;

    for (int i = 0; i < APDUlen; i++) {
        PDU[i+7] = APDU[i];
    }

    //open tcp connection
    struct sockaddr_in serv;

    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    inet_aton(server_add, &serv.sin_addr);

    int new_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (new_socket == -1) {
        printf("Socket was not not created\n");
        
        return -1;
    }

    int conection = connect(new_socket, (struct sockaddr*)&serv, sizeof(serv));
    if (conection != 0) {
        printf("Connection failed\n");
       
        return -1;
    }

    //send modbus tcp pdu
    write(new_socket, PDU, APDUlen+7); //+7 bytes MBAP

    int len_r = read(new_socket, PDU_R, 7);
    if (len_r != 7)
    {
        
         return -1;
    } 
    if(PDU_R[0] != PDU[0]){
        
         return -1;
    }
    if(PDU_R[1] != PDU[1]){
       ; 
        return -1;
    }
    if(PDU_R[2] != 0){
        
        return -1;
    }
    if(PDU_R[3] != 0){
         
        return -1;
    }
    if(PDU_R[6] != PDU[6]){
       
        return -1;
    }

    uint16_t APDU_len_r = (PDU_R[4]/256 & PDU_R[5])-1;
    len_r = read(new_socket, APDU_R, APDU_len_r);
    if (len_r != APDU_len_r){
        
        return -1;
    }

    close(new_socket);
    
    return 0;

}
    
    
    
