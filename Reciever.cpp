//Includes all the required header files and structure definition of frame
#include "protocol.h"

class Reciever{ // acts as server
    public:
        Reciever(int port){
            int frame_expected = 0;
            frame s,r;
            int socket_desc, client_sock;
            struct sockaddr_in server_addr, client_addr;
            
            // Create socket:
            socket_desc = socket(AF_INET, SOCK_STREAM, 0);
            
            if(socket_desc < 0){
                cout<<"Error while creating socket"<<endl;
                return ;
            }
            cout<<"Socket created successfully"<<endl;
            
            // Set port and IP:
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(port);
            server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            
            // Bind to the set port and IP:
            if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
                cout<<"Couldn't bind to the port"<<endl;
                return ;
            }
            cout<<"Done with binding"<<endl;
            
        
            if(listen(socket_desc, 1) < 0){
                cout<<"Error while listening"<<endl;
                return ;
            }
            cout<<endl<<"Listening for incoming connections....."<<endl;
            
        
            socklen_t client_size = sizeof(client_addr);
            client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
            cout<<"Connection established"<<endl;

            if (client_sock < 0){
                cout<<"Can't accept"<<endl;
                return ;
            }
            while(true){
                cout<<endl;
                // recieve a frame
                if (recv(client_sock, (void *)(&r), sizeof(r), 0) > 0){
                    if(r.seq_num == frame_expected){
                        // frame recieved correctly
                        cout<<"Msg from client: "<<r.data<<endl; // pass the recieved frame to the Network Layer
                        frame_expected ^= 1;
                    } else {
                        // frame is either incorrect or sent repeatedly
                        cout<<"Wrong Sequential Number recieved"<<endl<<"Sending acknowledgement"<<endl;
                    }
                    s.ack_num = 1 - frame_expected;
                    s.event_type = ACK;
                }
                else{
                    break;
                }
                // simulation of error introduced during transmission
                // error introduced with probability of 0.1
                if(rand()%10 == 1 ){
                    cout<<"Introducing ERROR in reciever"<<endl;
                    s.ack_num ^= 1;
                }
                // send the acknowledgement
                if (send(client_sock, (void *)(&s), sizeof(s), 0) < 0){
                    cout<<"Can't send"<<endl;
                }
            }    
            // Closing the socket:
            close(client_sock);
            close(socket_desc);
        }
};

int main(){
    srand(time(0));
    Reciever R(5002);
    return 0;
}