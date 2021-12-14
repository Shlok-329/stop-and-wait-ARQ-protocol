//Includes all the required header files and structure definition of frame
#include "protocol.h"

class Sender{ // acts like client
    public:
        Sender(int port){
            int next_frame_to_send = 0;
            frame s;
            int socket_desc;
            struct sockaddr_in server_addr;
            char client_message[MESSAGE_LEN];
            
            // Clean buffers:
            memset(client_message,'\0',sizeof(client_message));
            
            // Create socket:
            socket_desc = socket(AF_INET, SOCK_STREAM, 0);
            
            if(socket_desc < 0){
                cout<<"Unable to create socket"<<endl;
                return ;
            }
            
            cout<<"Socket created successfully"<<endl;
            
            // Set port and IP the same as reciever-side:
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(port);
            server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            
            // Send connection request to reciever:
            if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
                cout<<"Unable to connect"<<endl;
                return ;
            }
            cout<<"Connected with reciever successfully"<<endl<<endl;

            // Get input from the user:
            cout<<"Enter message: ";
            cin.getline(client_message,MESSAGE_LEN);

            while(true){
                // prepare the frame to send
                strcpy(s.data,client_message);
                s.event_type = DATA;
                s.seq_num = next_frame_to_send;
                // simulation of error introduced during transmission
                // error introduced with probability of 0.2
                if(rand()%5 == 0 ){
                    cout<<"Introducing ERROR in sender"<<endl;
                    s.seq_num ^= 1;
                }
                // Send the frame to the reciever
                if(send(socket_desc, (void *)(&s), sizeof(s), 0) < 0){
                    cout<<"Unable to send message"<<endl;
                    continue;
                }
                fd_set rfds;
                timeval tv;
                int retval;
                tv.tv_sec = 5;  //Waiting for acknowledgment for atmost 5 seconds
                tv.tv_usec = 0;
                FD_ZERO(&rfds);
                FD_SET(socket_desc, &rfds);
                // timer is set and socket is waiting for acknowledgement
                // whichever happens first will interrupt the following blocking function
                retval = select(socket_desc+1, &rfds, NULL, NULL, &tv);
                if(retval == 0){ // timeout
                    cout<<"Timeout. Sending frame again."<<endl;
                    continue; // send the frame again
                }
                // Receive the acknowledgement
                if(recv(socket_desc, (void *)(&s), sizeof(s), 0) > 0){
                    if(s.ack_num == next_frame_to_send){
                        next_frame_to_send ^= 1; // increment the seq number
                        cout<<"Acknowledgement recieved."<<endl<<endl<<"Enter message: ";
                        cin.getline(client_message,MESSAGE_LEN);
                    } else {
                        // acknowledgement incorrect, we will resend the frame
                        cout<<"Acknowledgement mismatch."<<endl<<"Sending the frame again."<<endl;
                    }
                } else {
                    break;
                }

            }
            // Close the socket:
            close(socket_desc);
        }
};

int main(){
    //Initializing the random seed   
    srand(time(0)+rand());
    Sender R(5002);
    return 0;
}