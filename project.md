Flock is a Multi-threaded terminal based chat application

**The project is based on Object Oriented Programming (OOP) principles**
**The Project code has to be simple and easy to understand**

1. General Features
    ├─ Chatting 1to1 realtime
    ├─ Chatting Group realtime
    ├─ multiuser connecting handling (multiple user can chat at the same time)
    

2. Multi-threaded Terminal User Interface Rendering

for every screen there will be a class in a .hpp file and a .cpp file, each sceen has multiple threads, and mutex guards so make the sceen render perfectly without any ascii collision
    
  UI Structure
    ├─ Loading / Splash Screen
    └─ Main Menu(after successful connection)
        ├─ Login Screen 
        ├─ Register ?/? Create Account Screen
        └─ Home/Chat List (after login)
            └─ ChatRoom

    the codes are very simple and easy to understand
    there is a 

3. The whole project is splited into 2 parts:

   Flock   
    ├─ Client Side
    │   ├─ UI
    │   ├─ Cryptography (using OpenSSL)
    │   ├─ Network (TCP Sockets)
    │   ├─ CMakeLists.txt (for client)
    │   └─ main.cpp (for client)
    │
    └─ Server Side (TCP Sockets)
        ├─ Database (simulated through file operations)
        ├─ Cryptography (using OpenSSL) // maybe i dont need it
        ├─ Network (TCP Sockets)
        ├─ CMakeLists.txt (for server)
        └─ main.cpp (for server)

so basically server has to be running in case of client can connect to it


4. Cryptography (using OpenSSL)

will have encryption and decryption for messages, user data, and chat data so encrypted datas will be stored in database folder ....



5. Database (simulated through file operations)

    Server/Database
        ├─ users.txt 
        ├─ rooms.txt 
        ├─ chats_1-1
        │   ├─ <userA>_<userB>.txt
        │   ├─ <userC>_<userD>.txt
        │    ...
        ├─ room_chats
            ├─ <roomname><roomid>.txt
             ...
        
 i. users.txt: 
    Stores user credentials for the Login and Register screens. When a user logs in, the server simply reads this file line-by-line to verify the credentials.
    
    data: userID|username|pass_hashed

 ii. rooms.txt: 
    Maps group identities to their members for Group Chatting 
                         
    data: roomID|roomName|userID_1,userID_2,userID_3

iii. <userA>_<userB>.txt : 
    Create a separate text file for every pair of users. To ensure consistency, sort the UserIDs alphabetically for the filename (e.g., if User A and User B chat, the file is always userA_userB.txt). Appending a new message is just opening one specific file in append mode. Loading chat history is just reading that one file.
                         
    data: timestamp|senderID|encryptedMessage

iv. <roomname><roomid>.txt: 
    Similar to 1-to-1 chats, appending and reading history is isolated to a single file, reducing I/O conflicts between different active groups.
                        
    data: Timestamp|SenderID|encryptedMessage

